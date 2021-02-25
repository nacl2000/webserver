#include "redis.h"

void redis_pool::init_redis_pool( int &max_conn ){
    m_max_conn = max_conn;
    m_free_conn = 0;
    for( int i = 0; i < max_conn; ++i ){
        redisContext *c = redisConnect( "127.0.0.1", 6379 );
        if( c->err ){
            redisFree( c );
            printf( "connect to redis faile\n" );
            continue;
        }
        m_free_conn++;
        connlist.push_back( c );
    }
    sum = sem( m_free_conn );
}

void redis_pool::destory_redis_pool(){
    for( auto i : connlist ){
        redisFree( i );
    }
    connlist.clear();
    m_max_conn = 0;
}

redisContext *redis_pool::get_connection(){
    lock.lock();
    sum.wait();
    redisContext *res = connlist.front();
    connlist.pop_front();
    lock.unlock();
    return res;
}

void redis_pool::free_connection( redisContext *c ){
    lock.lock();
    connlist.push_back( c );
    sum.post();
    lock.unlock();
}


