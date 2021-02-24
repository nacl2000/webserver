#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <list>
#include "server.h"
#include "lock.h"
#include <mysql/mysql.h>

class connect_pool{
    public:
        connect_pool( const char *url, const char *user, const char *password, 
                        const char *dbname, const int port, const int max_conn );
        ~connect_pool();
        static MYSQL *get_connection();
        static void release_connection(MYSQL *_sql);
        static locker table_lock;
        static locker lock;
        static sem sum;
        static std::list<MYSQL*> connList;
    
    private:
        static int m_max_conn;
        //static int m_curconn;
        static int m_free_conn;
};

#endif
