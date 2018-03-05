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

int addfd( int epoll_fd, int fd, bool oneshot ) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if ( oneshot ) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
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

void * myDownload::send_file( void *arg ) {
    
}

void * myDownload::test( void * arg ) {
    myDownload *temp = (myDownload *)arg;  //调用函数传入的对象
    int sock_fd = temp->sock_fd;
    int n = 0, ret = 0;
    char tempname[10];
    int r = recv( sock_fd, &n, sizeof( n ), 0 );  //当前为文件第几部分
printf("recv_n = %d, r = %d\n", n, r);
    sprintf( tempname, "temp_%d", n );
    int file_fd = open( tempname, O_WRONLY | O_CREAT | O_TRUNC, 0774 );

    while( true ) {
        char buf[BUFFER_SIZE] = {0};
        ret = recv( sock_fd, buf, BUFFER_SIZE-1, 0 );
printf("ret = %d\n", ret);
        if ( ret < 0 ) {
            sleep(1);
            continue;
        } else if ( !ret ) {
            break;
        }
        printf("buf:\n%s\n", buf);
        write( file_fd, buf, BUFFER_SIZE );
    }

    close( file_fd );
}