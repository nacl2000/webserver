#include "http_conn.h"
#include "threadpool.h"
#include "lock.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 20
extern int addfd( int epollfd, int fd, bool one_shot );
extern int removefd( int epollfd, int fd );
void addsig( int sig, void( handler )(int), bool restart = true ){
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = handler;
    if( restart ){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}
void show_error( int connfd, const char* info ){
    printf( "%s", info );
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}
int main( int argc, char *argv[] ){
    const char *ip = argv[ 1 ];
    int port = atoi( argv[ 2 ] );

    addsig( SIGPIPE, SIG_IGN );
    threadpool< http_conn >* pool = NULL;
    try{
        pool = new threadpool< http_conn >;
    }catch(...){
        return 1;
    }

    http_conn *users = new http_conn[ MAX_FD ];
    assert( users );
    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );
    struct linger tmp ={ 1, 0 };
    setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret >= 0 );
    ret = listen( listenfd, 20 );
    assert( ret >= 0 );

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 20 );
    assert( epollfd != -1 );
    addfd( epollfd, listenfd, false );
    http_conn::m_epollfd = epollfd;
     
    while( 1 ){
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if( ( number < 0 ) && ( errno != EINTR ) ){
            printf( "epoll faailure\n" );
            break;
        }
        
        for( int i = 0; i < number; ++i ){
            int sockfd = events[ i ].data.fd;
            if( sockfd == listenfd ){
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlen );
                //printf( "connfd is %d\n",connfd );
                //printf( "m_user_count is %d\nMAX_FD is %d\n", http_conn::m_user_count, MAX_FD );
                if( connfd < 0 ){
                    printf( "errno is: %d\n",errno );
                    continue;
                }
                if( http_conn::m_user_count >= MAX_FD ){
                    show_error( connfd, "Internal server busy" );
                    continue;
                }
                char client_ip[ INET_ADDRSTRLEN ];
                //inet_ntop( AF_INET, &client_address.sin_addr, &client_ip, INET_ADDRSTRLEN );
                //printf( "client ip:%s port:%d\n", client_ip, ntohs(client_address.sin_port));
                users[ connfd ].init( connfd, client_address );
                //printf( "client ip:%s port:%d\n", client_address.sin_addr, client_address.sin_port );
            }else if( events[ i ].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) ){
                if( events[ i ].events & EPOLLERR ){
                    puts( "EPOLLERR" );
                }else if( events[ i ].events & EPOLLHUP ){
                    puts( "EPOLLHUP" );
                }else{
                    puts( "EPOLLRDHUP" );
                }
                users[ sockfd ].close_conn();
            }else if( events[ i ].events & EPOLLIN ){
                //printf( "epollin\n" );
                if( users[ sockfd ].read() ){
                    pool->append( users + sockfd );
                }else{
                    printf( "fail read\n" );
                    users[ sockfd ].close_conn();
                }
            }else if( events[ i ].events & EPOLLOUT ){
                printf( "epollout\n" );
                if( !users[ sockfd ].write() ){
                    printf( "fail write\n" );
                    users[ sockfd ].close_conn();
                }
            }else{

            }
        }
    }
    close( epollfd );
    close( listenfd );
    delete [] users;
    delete pool;
    return 0;
}
