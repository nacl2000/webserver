#ifndef _REDIS_POOL_
#define _REDIS_POOL_

#include <list>
#include "lock.h"
#include <hiredis/hiredis.h>
class redis_pool{
    public:
        redis_pool( int max_conn ){
            init_redis_pool( max_conn );
        }

        ~redis_pool(){
            destory_redis_pool();
        }

        redisContext *get_connection();

        void free_connection( redisContext *c );
        
    private:
        void init_redis_pool( int &max_conn );
        
        void destory_redis_pool();
        
        std::list<redisContext*> connlist;

        int m_max_conn;

        sem sum;

        locker lock;

        int m_free_conn;
};
#endif
