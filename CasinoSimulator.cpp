#include "splashkit.h"
#include <cstdlib>
#include <ctime>
#include <mariadb/mysql.h>

#ifdef _WIN32
#include <windows.h>
#endif

const int MAX_HISTORY = 100;
const int MAX_CARDS = 12;

enum menu_option
{
    PLAY_DICE = 1,
    PLAY_BLACKJACK,
    PLAY_SLOTS,
    SHOW_STATS,
    SHOW_HISTORY,
    QUIT
};

enum result_type
{
    WIN,
    LOSS,
    DRAW
};

struct player
{
    string name;
    int chips;
    int games_played;
    int games_won;
};

struct game_result
{
    string game_name;
    int bet;
    result_type result;
    int player_value;
    int computer_value;
};

// Function declarations
void print_menu();
void print_player(const player &player_data);
int get_bet(const player &player_data);
void save_result(game_result history[], int &history_count, string game_name, int bet, result_type result, int player_value,
                 int computer_value);
void print_history(const game_result history[], int history_count);
void play_dice(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id);
int draw_card();
int calculate_hand(const int cards[], int card_count);
void print_hand(const int cards[], int card_count);
void player_turn(int cards[], int &card_count);
void dealer_turn(int cards[], int &card_count);
void play_blackjack(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id);
string generate_slot_symbol();
int get_slot_multiplier(string symbol);
void play_slots(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id);
MYSQL *connect_database();
int get_or_create_player(MYSQL *database, player &player_data);
void update_player_database(MYSQL *database, int player_id, const player &player_data);
void save_result_to_database(MYSQL *database, int player_id, string game_type, result_type result, int chips_change);

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand(time(NULL));

    MYSQL *database = connect_database();

    if (database != nullptr)
    {
        write_line("Database connected successfully.");
    }

    player player_data;

    // Default values in case database connection fails
    player_data.chips = 500;
    player_data.games_played = 0;
    player_data.games_won = 0;

    game_result history[MAX_HISTORY];
    int history_count = 0;

    write_line("Welcome to Casino Night Simulator!");

    write("Enter your name: ");
    player_data.name = read_line();

    int player_id = get_or_create_player(database, player_data);

    if (player_id == -1)
    {
        write_line("Could not load player from database.");
    }
    else
    {
        write_line("Player loaded successfully.");
        write_line("Player database ID: " + to_string(player_id));
    }

    write_line("Your starting chips are " + to_string(player_data.chips));

    player *current_player = &player_data;

    int choice;

    do
    {
        print_menu();
        choice = convert_to_integer(read_line());

        switch (choice)
        {
        case PLAY_DICE:
            if (current_player != nullptr)
            {
                if (current_player->chips > 0)
                {
                    play_dice(*current_player, history, history_count, database, player_id);
                }
                else
                {
                    write_line("You do not have enough chips to play.");
                }
            }
            break;

        case PLAY_BLACKJACK:
            if (current_player != nullptr)
            {
                if (current_player->chips > 0)
                {
                    play_blackjack(*current_player, history, history_count, database, player_id);
                }
                else
                {
                    write_line("You do not have enough chips to play.");
                }
            }
            break;

        case PLAY_SLOTS:
            if (current_player != nullptr)
            {
                if (current_player->chips > 0)
                {
                    play_slots(*current_player, history, history_count, database, player_id);
                }
                else
                {
                    write_line("You do not have enough chips to play.");
                }
            }
            break;

        case SHOW_STATS:
            if (current_player != nullptr)
            {
                print_player(*current_player);
            }
            break;

        case SHOW_HISTORY:
            print_history(history, history_count);
            break;

        case QUIT:
            write_line("Thanks for playing!");
            break;

        default:
            write_line("Invalid menu option.");
            break;
        }

    } while (choice != QUIT);

    if (database != nullptr)
    {
        mysql_close(database);
    }

    return 0;
}

// Display main menu
void print_menu()
{
    write_line("");
    write_line("=== Casino Night Simulator ===");
    write_line("1. Play Dice");
    write_line("2. Play Blackjack");
    write_line("3. Play Slot Machine");
    write_line("4. Show player statistics");
    write_line("5. Show game history");
    write_line("6. Quit");
    write("Option: ");
}

// Display player statistics
void print_player(const player &player_data)
{
    write_line("");
    write_line("=== Player Statistics ===");
    write_line("Name: " + player_data.name);
    write_line("Chips: " + to_string(player_data.chips));
    write_line("Games played: " + to_string(player_data.games_played));
    write_line("Games won: " + to_string(player_data.games_won));
}

// Read and validate bet
int get_bet(const player &player_data)
{
    int bet;

    write("Enter bet: ");
    bet = convert_to_integer(read_line());

    while (bet <= 0 || bet > player_data.chips)
    {
        write_line("Invalid bet.");
        write("Enter a bet from 1 to " + to_string(player_data.chips) + ": ");
        bet = convert_to_integer(read_line());
    }

    return bet;
}

// Save game result
void save_result(game_result history[], int &history_count, string game_name, int bet, result_type result, int player_value, int computer_value)
{
    if (history_count < MAX_HISTORY)
    {
        history[history_count].game_name = game_name;
        history[history_count].bet = bet;
        history[history_count].result = result;
        history[history_count].player_value = player_value;
        history[history_count].computer_value = computer_value;

        history_count++;
    }
}

// Display history
void print_history(const game_result history[], int history_count)
{
    write_line("");
    write_line("=== Game History ===");

    if (history_count == 0)
    {
        write_line("No games have been played yet.");
        return;
    }

    for (int i = 0; i < history_count; i++)
    {
        write_line("");
        write_line("Game " + to_string(i + 1));
        write_line("Game type: " + history[i].game_name);
        write_line("Bet: " + to_string(history[i].bet));

        if (history[i].result == WIN)
        {
            write_line("Result: Win");
        }
        else if (history[i].result == LOSS)
        {
            write_line("Result: Loss");
        }
        else
        {
            write_line("Result: Draw");
        }

        if (history[i].game_name == "Dice")
        {
            write_line("Player guess: " + to_string(history[i].player_value));
            write_line("Dice value: " + to_string(history[i].computer_value));
        }
        else if (history[i].game_name == "Slots")
        {
            if (history[i].result == WIN)
            {
                write_line("Winning lines: " + to_string(history[i].player_value));
                write_line("Total winnings: " + to_string(history[i].computer_value));
            }
        }
        else
        {
            write_line("Player total: " + to_string(history[i].player_value));
            write_line("Dealer total: " + to_string(history[i].computer_value));
        }
    }
}

// Play Dice game
void play_dice(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id)
{
    write_line("");
    write_line("=== Dice Game ===");

    int bet = get_bet(player_data);
    int guess;

    write("Choose a number from 1 to 6: ");
    guess = convert_to_integer(read_line());

    while (guess < 1 || guess > 6)
    {
        write("Invalid guess. Choose a number from 1 to 6: ");
        guess = convert_to_integer(read_line());
    }

    int dice_value = rand() % 6 + 1;

    write_line("Dice result: " + to_string(dice_value));

    player_data.games_played++;

    if (guess == dice_value)
    {
        int winnings = bet * 4;

        player_data.chips += winnings;
        player_data.games_won++;

        write_line("Correct guess!");
        write_line("You won " + to_string(winnings) + " chips.");

        save_result(history, history_count, "Dice", bet, WIN, guess, dice_value);
        save_result_to_database(database, player_id, "Dice", WIN, winnings);
    }
    else
    {
        player_data.chips -= bet;

        write_line("Incorrect guess.");
        write_line("You lost " + to_string(bet) + " chips.");

        save_result(history, history_count, "Dice", bet, LOSS, guess, dice_value);
        save_result_to_database(database, player_id, "Dice", LOSS, -bet);
    }

    update_player_database(database, player_id, player_data);

    write_line("Current chips: " + to_string(player_data.chips));
}

// Draw one Blackjack card
int draw_card()
{
    int rank = rand() % 13 + 1;

    if (rank == 1)
    {
        return 11;
    }
    else if (rank >= 10)
    {
        return 10;
    }
    else
    {
        return rank;
    }
}

// Calculate Blackjack hand
int calculate_hand(const int cards[], int card_count)
{
    int total = 0;
    int ace_count = 0;

    for (int i = 0; i < card_count; i++)
    {
        total += cards[i];

        if (cards[i] == 11)
        {
            ace_count++;
        }
    }

    while (total > 21 && ace_count > 0)
    {
        total -= 10;
        ace_count--;
    }

    return total;
}

// Display cards
void print_hand(const int cards[], int card_count)
{
    for (int i = 0; i < card_count; i++)
    {
        write(to_string(cards[i]) + " ");
    }

    write_line("");
}

// Player turn
void player_turn(int cards[], int &card_count)
{
    string choice;
    bool finished = false;

    while (!finished && calculate_hand(cards, card_count) < 21)
    {
        write_line("");

        write("Your cards: ");
        print_hand(cards, card_count);
        write_line("Total: " + to_string(calculate_hand(cards, card_count)));

        write("Hit or Stand? (h/s): ");
        choice = read_line();

        while (choice != "h" && choice != "H" && choice != "s" && choice != "S")
        {
            write("Invalid choice. Enter h or s: ");
            choice = read_line();
        }

        if (choice == "h" || choice == "H")
        {
            if (card_count < MAX_CARDS)
            {
                cards[card_count] = draw_card();
                card_count++;
            }
            else
            {
                finished = true;
            }
        }
        else
        {
            finished = true;
        }
    }
}

// Dealer turn
void dealer_turn(int cards[], int &card_count)
{
    while (calculate_hand(cards, card_count) < 17 && card_count < MAX_CARDS)
    {
        cards[card_count] = draw_card();
        card_count++;
    }
}

// Play Blackjack
void play_blackjack(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id)
{
    write_line("");
    write_line("=== Blackjack ===");

    int bet = get_bet(player_data);

    int player_cards[MAX_CARDS];
    int dealer_cards[MAX_CARDS];

    int player_card_count = 2;
    int dealer_card_count = 2;

    player_cards[0] = draw_card();
    player_cards[1] = draw_card();

    dealer_cards[0] = draw_card();
    dealer_cards[1] = draw_card();

    write_line("");
    write_line("Dealer shows: " + to_string(dealer_cards[0]));

    player_turn(player_cards, player_card_count);

    int player_total = calculate_hand(player_cards, player_card_count);

    player_data.games_played++;

    if (player_total > 21)
    {
        write_line("");
        write("Your final cards: ");
        print_hand(player_cards, player_card_count);
        write_line("Your total: " + to_string(player_total));
        write_line("You busted!");

        player_data.chips -= bet;

        save_result(history, history_count, "Blackjack", bet, LOSS, player_total, calculate_hand(dealer_cards, dealer_card_count));
        save_result_to_database(database, player_id, "Blackjack", LOSS, -bet);

        update_player_database(database, player_id, player_data);

        write_line("Current chips: " + to_string(player_data.chips));
        return;
    }

    dealer_turn(dealer_cards, dealer_card_count);

    int dealer_total = calculate_hand(dealer_cards, dealer_card_count);

    write_line("");
    write("Your final cards: ");
    print_hand(player_cards, player_card_count);
    write_line("Your total: " + to_string(player_total));

    write("Dealer cards: ");
    print_hand(dealer_cards, dealer_card_count);
    write_line("Dealer total: " + to_string(dealer_total));

    if (dealer_total > 21)
    {
        int winnings = bet * 2;

        player_data.chips += winnings;
        player_data.games_won++;

        write_line("Dealer busted. You win!");
        write_line("You won " + to_string(winnings) + " chips.");

        save_result(history, history_count, "Blackjack", bet, WIN, player_total, dealer_total);
        save_result_to_database(database, player_id, "Blackjack", WIN, winnings);
    }
    else if (player_total > dealer_total)
    {
        int winnings = bet * 2;

        player_data.chips += winnings;
        player_data.games_won++;

        write_line("You win!");
        write_line("You won " + to_string(winnings) + " chips.");

        save_result(history, history_count, "Blackjack", bet, WIN, player_total, dealer_total);
        save_result_to_database(database, player_id, "Blackjack", WIN, winnings);
    }
    else if (player_total < dealer_total)
    {
        player_data.chips -= bet;

        write_line("Dealer wins.");
        write_line("You lost " + to_string(bet) + " chips.");

        save_result(history, history_count, "Blackjack", bet, LOSS, player_total, dealer_total);
        save_result_to_database(database, player_id, "Blackjack", LOSS, -bet);
    }
    else
    {
        write_line("It is a draw.");

        save_result(history, history_count, "Blackjack", bet, DRAW, player_total, dealer_total);
        save_result_to_database(database, player_id, "Blackjack", DRAW, 0);
    }

    update_player_database(database, player_id, player_data);

    write_line("Current chips: " + to_string(player_data.chips));
}

// Generate one Slot Machine symbol
string generate_slot_symbol()
{
    int random_number = rand() % 100;

    if (random_number < 20)
    {
        return "🍒";
    }
    else if (random_number < 38)
    {
        return "🍋";
    }
    else if (random_number < 54)
    {
        return "🔔";
    }
    else if (random_number < 68)
    {
        return "⭐";
    }
    else if (random_number < 80)
    {
        return "💎";
    }
    else if (random_number < 91)
    {
        return "👑";
    }
    else
    {
        return "7";
    }
}

// Return Slot Machine payout multiplier
int get_slot_multiplier(string symbol)
{
    if (symbol == "🍒")
    {
        return 2;
    }
    else if (symbol == "🍋")
    {
        return 3;
    }
    else if (symbol == "🔔")
    {
        return 4;
    }
    else if (symbol == "⭐")
    {
        return 6;
    }
    else if (symbol == "💎")
    {
        return 8;
    }
    else if (symbol == "👑")
    {
        return 12;
    }
    else if (symbol == "7")
    {
        return 20;
    }

    return 0;
}

// Play Slot Machine
void play_slots(player &player_data, game_result history[], int &history_count, MYSQL *database, int player_id)
{
    const int ROWS = 3;
    const int COLS = 3;

    string slots[ROWS][COLS];

    write_line("");
    write_line("=== Slot Machine ===");
    write_line("There are 8 winning paylines:");
    write_line("3 horizontal, 3 vertical and 2 diagonal.");

    write_line("");
    write_line("Payouts:");
    write_line("🍒 🍒 🍒 = x2");
    write_line("🍋 🍋 🍋 = x3");
    write_line("🔔 🔔 🔔 = x4");
    write_line("⭐ ⭐ ⭐ = x6");
    write_line("💎 💎 💎 = x8");
    write_line("👑 👑 👑 = x12");
    write_line("7  7  7  = x20");

    write_line("");
    write_line("Multi-line bonus:");
    write_line("2 winning lines = x2 total winnings");
    write_line("3 winning lines = x3 total winnings");
    write_line("4 or more winning lines = x5 total winnings");

    int bet = get_bet(player_data);

    // Generate Slot Machine symbols
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            slots[row][col] = generate_slot_symbol();
        }
    }

    // Display Slot Machine
    write_line("");
    write_line("=======================");

    for (int row = 0; row < ROWS; row++)
    {
        write("| ");

        for (int col = 0; col < COLS; col++)
        {
            write(slots[row][col] + " | ");
        }

        write_line("");
    }

    write_line("=======================");

    player_data.games_played++;

    int paylines[8][6] =
        {
            {0, 0, 0, 1, 0, 2},
            {1, 0, 1, 1, 1, 2},
            {2, 0, 2, 1, 2, 2},

            {0, 0, 1, 0, 2, 0},
            {0, 1, 1, 1, 2, 1},
            {0, 2, 1, 2, 2, 2},

            {0, 0, 1, 1, 2, 2},
            {0, 2, 1, 1, 2, 0}};

    int winning_lines = 0;
    int total_winnings = 0;

    // Check winning paylines
    for (int i = 0; i < 8; i++)
    {
        int row1 = paylines[i][0];
        int col1 = paylines[i][1];

        int row2 = paylines[i][2];
        int col2 = paylines[i][3];

        int row3 = paylines[i][4];
        int col3 = paylines[i][5];

        string symbol1 = slots[row1][col1];
        string symbol2 = slots[row2][col2];
        string symbol3 = slots[row3][col3];

        if (symbol1 == symbol2 && symbol2 == symbol3)
        {
            int multiplier = get_slot_multiplier(symbol1);
            int line_winnings = bet * multiplier;

            winning_lines++;
            total_winnings += line_winnings;

            write_line("");
            write_line("Winning line " + to_string(i + 1) + "!");
            write_line("Symbol: " + symbol1);
            write_line("Multiplier: x" + to_string(multiplier));
            write_line("Line winnings: " + to_string(line_winnings) + " chips.");
        }
    }

    // Apply multi-line bonus
    int bonus_multiplier = 1;

    if (winning_lines == 2)
    {
        bonus_multiplier = 2;
    }
    else if (winning_lines == 3)
    {
        bonus_multiplier = 3;
    }
    else if (winning_lines >= 4)
    {
        bonus_multiplier = 5;
    }

    if (winning_lines > 0)
    {
        if (bonus_multiplier > 1)
        {
            write_line("");
            write_line("Multi-line bonus: x" + to_string(bonus_multiplier));
            total_winnings *= bonus_multiplier;
        }

        player_data.chips += total_winnings;
        player_data.games_won++;

        write_line("");
        write_line("You win!");
        write_line("Winning lines: " + to_string(winning_lines));
        write_line("Total winnings: " + to_string(total_winnings) + " chips.");

        save_result(history, history_count, "Slots", bet, WIN, winning_lines, total_winnings);
        save_result_to_database(database, player_id, "Slots", WIN, total_winnings);
    }
    else
    {
        player_data.chips -= bet;

        write_line("");
        write_line("No winning lines.");
        write_line("You lost " + to_string(bet) + " chips.");

        save_result(history, history_count, "Slots", bet, LOSS, 0, 0);
        save_result_to_database(database, player_id, "Slots", LOSS, -bet);
    }

    update_player_database(database, player_id, player_data);

    write_line("Current chips: " + to_string(player_data.chips));
}

// Connect to MariaDB
MYSQL *connect_database()
{
    _putenv("MARIADB_TLS_DISABLE_PEER_VERIFICATION=1");

    MYSQL *connection = mysql_init(nullptr);

    if (connection == nullptr)
    {
        write_line("Database initialization failed.");
        return nullptr;
    }

    bool verify_ssl = false;
    mysql_options(connection, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify_ssl);

    MYSQL *result = mysql_real_connect(connection, "127.0.0.1", "root", "", "casino sit102", 3306, nullptr, 0);

    if (result == nullptr)
    {
        write_line("Database connection failed: " + string(mysql_error(connection)));
        mysql_close(connection);
        return nullptr;
    }

    return connection;
}

// Get existing player or create a new player
int get_or_create_player(MYSQL *database, player &player_data)
{
    if (database == nullptr)
    {
        return -1;
    }

    string query = "SELECT PLAYER_ID, CHIPS, GAME_PLAYED, GAMES_WON FROM PLAYER WHERE NAME = '" + player_data.name + "'";

    if (mysql_query(database, query.c_str()) != 0)
    {
        write_line("Database query failed: " + string(mysql_error(database)));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(database);

    if (result != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(result);

        if (row != nullptr)
        {
            int player_id = convert_to_integer(row[0]);

            player_data.chips = convert_to_integer(row[1]);
            player_data.games_played = convert_to_integer(row[2]);
            player_data.games_won = convert_to_integer(row[3]);

            mysql_free_result(result);

            return player_id;
        }

        mysql_free_result(result);
    }

    query = "INSERT INTO PLAYER (NAME, CHIPS, GAME_PLAYED, GAMES_WON, TOTAL_WINS) VALUES ('" + player_data.name + "', 500, 0, 0, 0)";

    if (mysql_query(database, query.c_str()) != 0)
    {
        write_line("Could not create player: " + string(mysql_error(database)));
        return -1;
    }

    player_data.chips = 500;
    player_data.games_played = 0;
    player_data.games_won = 0;

    return mysql_insert_id(database);
}

// Update player data in database
void update_player_database(MYSQL *database, int player_id, const player &player_data)
{
    if (database == nullptr || player_id == -1)
    {
        return;
    }

    string query = "UPDATE PLAYER SET CHIPS = " + to_string(player_data.chips) +
                   ", GAME_PLAYED = " + to_string(player_data.games_played) +
                   ", GAMES_WON = " + to_string(player_data.games_won) +
                   " WHERE PLAYER_ID = " + to_string(player_id);

    if (mysql_query(database, query.c_str()) != 0)
    {
        write_line("Database update failed: " + string(mysql_error(database)));
    }
}
void save_result_to_database(MYSQL *database, int player_id, string game_type, result_type result, int chips_change)
{
    if (database == nullptr || player_id == -1)
    {
        return;
    }

    string result_text;

    if (result == WIN)
    {
        result_text = "WIN";
    }
    else if (result == LOSS)
    {
        result_text = "LOSS";
    }
    else
    {
        result_text = "DRAW";
    }

    string query = "INSERT INTO GAME_RESULT (PLAYER_ID, GAME_TYPE, RESULT, CHIPS_CHANGE) VALUES (" +
                   to_string(player_id) + ", '" +
                   game_type + "', '" +
                   result_text + "', " +
                   to_string(chips_change) + ")";

    if (mysql_query(database, query.c_str()) != 0)
    {
        write_line("Could not save game result: " + string(mysql_error(database)));
    }
}