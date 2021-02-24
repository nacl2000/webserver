#include <mysql/mysql.h>
#include <stdio.h>
int main(){
    MYSQL sql;
    mysql_init( &sql );
    mysql_real_connect( &sql, "localhost", "root", "jhy,.jht666", "nacl", 0, NULL, 0 );
    printf( "hello\n" );
    return 0;
}
