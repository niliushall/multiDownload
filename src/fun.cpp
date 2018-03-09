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

int tell( int fd ) {
    return lseek( fd, 0, SEEK_CUR );
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
    myDownload file;  //接收
    int sock_fd = temp->sock_fd;
    int ret = 0;
    char tempname[10];
    
    int r = recv( sock_fd, &file, sizeof( file ), 0 );

    /* 已接收完毕 */
    if ( ! r ) {  //合并文件
        char filename[100];
        sprintf( filename, "%s_download", temp->filename );
        int fd = open( filename, O_CREAT | O_RDWR | O_TRUNC, 0774 );

        for( int i = 0; i < temp->info.num; i++ ) {
            sprintf( tempname, "temp_%d", i );
cout << tempname << endl;
            off_t offset = i * temp->info.size;
cout << "offset = " << offset << endl;
            lseek( fd, offset, SEEK_SET );

            int file_fd = open( tempname, O_RDONLY );
            int end = lseek( file_fd, 0, SEEK_END );  //获取文件大小
            lseek( file_fd, 0, SEEK_SET );  //移到文件开始
            
            while( tell( file_fd ) < end ) {
                char buf[ BUFFER_SIZE ] = {0};
                read( file_fd, buf, BUFFER_SIZE );
// cout << buf << endl;
                write( fd, buf, strlen( buf ) );
            }
            close( file_fd );
        }
        close( fd );
        cout << "finish\n";
        exit( 0 );
    }
// printf("i = %d\n", file.info.i);
    temp->info.size = file.info.size;
    sprintf( tempname, "temp_%d", file.info.i );
    int file_fd = open( tempname, O_WRONLY | O_CREAT | O_APPEND, 0774 );
    write( file_fd, file.info.buf, strlen( file.info.buf ) );
    close( file_fd );
}