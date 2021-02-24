#include "mysql.h"

connect_pool::connect_pool(const char *url, const char *usr, const char *password, const char *dbname
                            ,const int port, const int max_conn){
    m_max_conn = max_conn;
    m_free_conn = 0;
    for( int i = 0; i < max_conn; ++i ){
        MYSQL *con = NULL;
        con = mysql_init( con );

        if( con == NULL ){
            printf( "init fail\n" );
            exit( 1 );
        }
        con = mysql_real_connect( con, url, usr, password, dbname, port, NULL, 0 );
        
        if( con == NULL ){
            printf( "connect fail\n" );
            exit(1);
        }
        connList.push_back( con );
        ++m_free_conn;
    }
    sum = sem( m_free_conn );
}

connect_pool::~connect_pool(){
    lock.lock();
    for( auto i:connList ){
        MYSQL *con = i;
        mysql_close( con );
    }
    //m_curconn = 0;
    m_free_conn = 0;
    connList.clear();
    lock.unlock();
}
MYSQL *connect_pool::get_connection(){
    lock.lock();
    sum.wait();
    MYSQL *con = connList.front();
    connList.pop_front();
    m_free_conn--;
    return con;
}
void connect_pool::release_connection( MYSQL *_sql ){
    lock.lock();
    connList.push_back( _sql );
    ++m_free_conn;
    lock.unlock();
    sum.post();
    return;
}
