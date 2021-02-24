#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <iostream>
int main(){
    MYSQL sql;
    MYSQL *con=mysql_init( &sql );
    if( con == NULL ){
        int ret = mysql_errno( &sql );
        printf( "init errno is %d\n",ret );
    }
    if( mysql_real_connect( con, "localhost", "root", "jhy,.jht666", "nacl", 3306, NULL, 0 )==NULL){
        int ret = mysql_errno( &sql );
        printf( "connect errno is %d\n",ret );
        return 0;
    }
    mysql_query( con, "select password from user where name='jht';" );
    MYSQL_RES *result = mysql_store_result( con );
    MYSQL_ROW row = mysql_fetch_row( result );
    if( row == NULL ){
        puts("null");
        return 0;
    }
    std::cout<<std::string( row[ 0 ] )<<'\n';
    printf( "hello\n" );
    return 0;
}
