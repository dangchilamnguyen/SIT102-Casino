#include <iostream>
#include <cstdlib>
#include <mariadb/mysql.h>

using namespace std;

int main()
{
    // Workaround for MariaDB Connector/C 3.4.x with local non-TLS server
    _putenv("MARIADB_TLS_DISABLE_PEER_VERIFICATION=1");

    MYSQL *connection = mysql_init(nullptr);

    if (connection == nullptr)
    {
        cout << "mysql_init failed." << endl;
        return 1;
    }

    bool verify_ssl = false;
    mysql_options(connection, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify_ssl);

    MYSQL *result = mysql_real_connect(
        connection,
        "127.0.0.1",
        "root",
        "",
        "casino sit102",
        3306,
        nullptr,
        0
    );

    if (result == nullptr)
    {
        cout << "Connection failed: "
             << mysql_error(connection) << endl;

        mysql_close(connection);
        return 1;
    }

    cout << "Connected to database successfully!" << endl;

    mysql_close(connection);

    return 0;
}