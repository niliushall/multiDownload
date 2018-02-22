/* 客户端函数 */

#include "server.h"

void err( int line ) {
    cout << "error_line : " << line << endl;
}

int setnonblocking( int fd ) {
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

int addfd( int epoll_fd, int fd ) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking(fd);
}

myDownload::myDownload() {

}

// 客户端接收信息线程函数
void * myDownload::download( void * arg ) {
    myDownload *temp = (myDownload *)arg;  //调用函数传入的对象
    myDownload tempfile;  //recv接收的对象
    int sock_fd = temp->sock_fd;
    int ret;
    while( true ) {
        char filename[100];
        ret = recv( sock_fd, &tempfile, sizeof( tempfile ), 0 );
        sprintf( filename, "temp_%d", tempfile.info.i );  //获取临时文件名
        if ( ret <= 0 ) {
            break;
        }
        
        int file_fd = open( filename, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU );
        write( file_fd, tempfile.info.buf, sizeof( tempfile.info.buf ) );
        close( file_fd );
    }
}