/* 客户端函数 */

#include "server.h"

void err( int line ) {
    cout << "error_line : " << line << endl;
}

int myDownload::setnonblocking( int fd ) {
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

int myDownload::addfd( int epoll_fd, int fd ) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking(fd);
}

myDownload::myDownload() {

}

void * myDownload::download( void * arg ) {
    int sock_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_fd < 0 ) {
        err(__LINE__);
    }

    int ret = connect( sock_fd, ( struct sockaddr * )&(( ( myDownload * )arg )->address),
                        sizeof( ( ( myDownload * )arg )->address ) );
    if ( ret < 0 ) {
        err( __LINE__ );
    }

    // file_info file;
    // file.filename = ( ( myDownload * )arg )->filename;

    // int sock_fd = ( ( myDownload * )arg )->sock_fd;
    // int epoll_fd = ( ( myDownload * )arg )->epoll_fd;
    char buf[BUFFER_SIZE] = {0};
    
    send( sock_fd, ( myDownload * )arg, sizeof( ( myDownload * )arg ), 0 );
}