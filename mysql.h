#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "lock.h"
#include <pthread.h>
class mysql_pool{
    public:
        mysql_pool(){}
        ~mysql_pool(){}
        void init( const char *url, const char *user, const char *password, 
                        const char *dbname, const int port, const int max_conn );
        void destory_pool();
        MYSQL *get_connection();
        locker table_lock;
        void release_connection(MYSQL *_sql);
    private:
        locker sql_lock;
        sem sum;
        std::list<MYSQL*> connList;    
        int m_max_conn;
        //static int m_curconn;
        int m_free_conn;
};
#endif
