#include "splashkit.h"
#include <cstdlib>
#include <ctime>

const int MAX_HISTORY = 100;
const int MAX_CARDS = 12;

enum menu_option
{
    PLAY_DICE = 1,
    PLAY_BLACKJACK,
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
void save_result(game_result history[], int &history_count, string game_name, int bet, result_type result,
                 int player_value, int computer_value);
void print_history(const game_result history[], int history_count);
void play_dice(player &player_data, game_result history[], int &history_count);
int draw_card();
int calculate_hand(const int cards[], int card_count);
void print_hand(const int cards[], int card_count);
void player_turn(int cards[], int &card_count);
void dealer_turn(int cards[], int &card_count);
void play_blackjack(player &player_data, game_result history[], int &history_count);

int main()
{
    srand(time(NULL));

    player player_data;

    player_data.chips = 500;
    player_data.games_played = 0;
    player_data.games_won = 0;

    game_result history[MAX_HISTORY];
    int history_count = 0;

    write_line("Welcome to Casino Night Simulator!");

    write("Enter your name: ");
    player_data.name = read_line();

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
                    play_dice(*current_player, history, history_count);
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
                    play_blackjack(*current_player, history, history_count);
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

    return 0;
}

// Display main menu
void print_menu()
{
    write_line("");
    write_line("=== Casino Night Simulator ===");
    write_line("1. Play Dice");
    write_line("2. Play Blackjack");
    write_line("3. Show player statistics");
    write_line("4. Show game history");
    write_line("5. Quit");
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
void save_result(game_result history[], int &history_count, string game_name, int bet,
                 result_type result, int player_value, int computer_value)
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
        else
        {
            write_line("Player total: " + to_string(history[i].player_value));
            write_line("Dealer total: " + to_string(history[i].computer_value));
        }
    }
}

// Play Dice game
void play_dice(player &player_data, game_result history[], int &history_count)
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
    }
    else
    {
        player_data.chips -= bet;

        write_line("Incorrect guess.");
        write_line("You lost " + to_string(bet) + " chips.");

        save_result(history, history_count, "Dice", bet, LOSS, guess, dice_value);
    }

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
void play_blackjack(player &player_data, game_result history[], int &history_count)
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

        save_result(history, history_count, "Blackjack", bet, LOSS, player_total,
                    calculate_hand(dealer_cards, dealer_card_count));

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
    }
    else if (player_total > dealer_total)
    {
        int winnings = bet * 2;

        player_data.chips += winnings;
        player_data.games_won++;

        write_line("You win!");
        write_line("You won " + to_string(winnings) + " chips.");

        save_result(history, history_count, "Blackjack", bet, WIN, player_total, dealer_total);
    }
    else if (player_total < dealer_total)
    {
        player_data.chips -= bet;

        write_line("Dealer wins.");
        write_line("You lost " + to_string(bet) + " chips.");

        save_result(history, history_count, "Blackjack", bet, LOSS, player_total, dealer_total);
    }
    else
    {
        write_line("It is a draw.");

        save_result(history, history_count, "Blackjack", bet, DRAW, player_total, dealer_total);
    }

    write_line("Current chips: " + to_string(player_data.chips));
}