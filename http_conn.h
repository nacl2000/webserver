#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "server.h"
#include <cstdio>
#include <sys/uio.h>
#include "lock.h"
class http_conn{
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    /*http请求方法*/
    enum METHOD { GET = 0, POST, CONNECT };
    /*解析客户请求时，主机所处的状态*/
    enum CHECK_STATE{ CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    /*服务器处理http请求的可能结果*/
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST,
                    NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST,
                    INTERNAL_ERROR, CLOSED_CONNECTION };
    /*行的读取状态*/
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };
    
    public:
        http_conn(){}
        ~http_conn(){}

    public:
        void init( int sockfd, const sockaddr_in& addr );

        void close_conn( bool real_close = true );

        void process();

        bool read();

        bool write();

    private:
        void init();

        HTTP_CODE process_read();

        bool process_write( HTTP_CODE ret );

        HTTP_CODE parse_request_line( char* text );
        HTTP_CODE parse_headers( char* text );
        HTTP_CODE parse_content( char* text );
        HTTP_CODE do_request();
        char *get_line(){ return m_read_buf + m_start_line; }
        LINE_STATUS parse_line();

        void unmap();
        bool add_response( const char* format,... );
        bool add_content( const char* content );
        bool add_status_line( int status, const char* title );
        bool add_headers( int content_length );
        bool add_content_length( int content_length );
        bool add_linger();
        bool add_blank_line();

    public:
        static int m_epollfd;

        static int m_user_count;

    private:
        int m_sockfd;

        sockaddr_in m_address;
        //读缓冲区
        char m_read_buf[ READ_BUFFER_SIZE ];
        //读入客户数据的最后一个字节的下一个位置
        int m_read_idx;
        //正在分析的字符在缓冲区的位置
        int m_checked_idx;
        //当前解析行在缓冲区起始的位置
        int m_start_line;
        //写缓冲区
        char m_write_buf[ WRITE_BUFFER_SIZE ];
        //写缓冲区待发送的字节数
        int m_write_idx;
        //主机当前状态
        CHECK_STATE m_check_state;
        //请求方法
        METHOD m_method;
        //客户请求目标文件的完整路径
        char m_real_file[ FILENAME_LEN ];
        //客户请求的文件名
        char *m_url;
        //http协议版本号
        char *m_version;
        //主机名
        char *m_host;
        //http请求的消息体的长度
        int m_content_length;
        //http请求是否要保连接
        bool m_linger;
        //客户请求文件被mmap到内存中的起使位置
        char *m_file_address;
        //目标文件的状态
        struct stat m_file_stat;
        //内存位置
        struct iovec m_iv[ 2 ];
        //内存块数量
        int m_iv_count;

        char s[25];

        int s_len;
};
#endif

