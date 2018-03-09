#include "server.h"

int main( int argc, char * argv[] ) {
    if ( argc < 4 ) {
        cout << "usage: ./client  ip_num  filename  thread_num\n";
        return -1;
    }

    myDownload file;  //待下载文件信息
    strcpy( file.filename, argv[2] );
    const char * ip = argv[1];
    file.info.num = atoi( argv[3] );

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons( PORT );
    inet_pton( AF_INET, ip, &address.sin_addr );

    int sock_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_fd < 0 ) {
        err(__LINE__);
    }
    file.sock_fd = sock_fd;

    int ret = connect( sock_fd, ( struct sockaddr * )&address,
                        sizeof( address ) );
    if ( ret < 0 ) {
        err( __LINE__ );
    }

    /* epoll */
    epoll_event events[ MAX_EVENT_NUMBER ];  //内核事件表信息
    int epoll_fd = epoll_create( 5 );
    if ( epoll_fd < 0 ) {
        err( __LINE__ );
    }

    addfd( epoll_fd, sock_fd, false );

    pthread_t threads[MAX_THREAD_NUM];
    int count = 0;  //记录新开线程数
    //epoll监听

    send( sock_fd, &file, sizeof( file ), 0 );  //发送所需文件信息

    while( true ) {
        ret = epoll_wait( epoll_fd, events, MAX_EVENT_NUMBER, -1 );
        if ( ret < 0 ) {
            cout << "epoll_wait failure\n";
            break;
        }

        for( int i = 0; i < ret; i++ ) {
            int fd = events[i].data.fd;
            if ( events[i].events & EPOLLIN ) {  //有新文件信息回传
                pthread_create( &threads[count++], NULL, file.test, (void *)&file );
            } else if ( events[i].events & EPOLLRDHUP ) {
                cout << "EPOLLRDHUP\n";
            }
        }
    }

    //等待线程结束
    for( int i = 0; i < count; i++ ) {
        pthread_join( threads[i], NULL );
    }

    //合并文件
    // for( int i = 0; i < file.info.num; i++ ) {
        
    // }

    cout << "download file " << argv[2] << " success\n";
    close( sock_fd );

    return 0;
}