#include "server.h"

int main(int argc, char * argv[]) {
    const char *ip = argv[1];
    struct sockaddr_in address;
    memset( &address, 0, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_port = htons( PORT );
    inet_pton( AF_INET, ip, &address.sin_addr );

    int sock_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_fd <= 0 ) {
        err( __LINE__ );
    }

    int ret = bind( sock_fd, ( struct sockaddr * )&address, sizeof( address ) );
    if ( ret < 0 ) {
        err( __LINE__ );
    }

    ret = listen( sock_fd, 5 );
    if ( ret < 0 ) {
        err( __LINE__ );
    }

    epoll_event events[MAX_EVENT_NUMBER];
    int epoll_fd = epoll_create( 5 );
    if ( epoll_fd < 0 ) {
        err( __LINE__ );
    }
    addfd( epoll_fd, sock_fd, false );

    while( true ) {
        ret = epoll_wait( epoll_fd, events, MAX_EVENT_NUMBER, -1 );
        if ( ret < 0 ) {
            cout << "epoll_wait error\n";
            return -1;
        }

        for( int i = 0; i < ret; i++ ) {
            int fd = events[i].data.fd;
            if ( fd == sock_fd ) {  //新连接
                struct sockaddr_in client;
                socklen_t client_length = sizeof( client );
                int conn_fd = accept( sock_fd, ( struct sockaddr * )&client,
                                        &client_length );
                if ( conn_fd <= 0 ) {
                    cout << "accept error\n";
                    return -1;
                }
                addfd( epoll_fd, conn_fd, true );
            } else if ( events[i].events & EPOLLIN ) {  //有信息传入
                myDownload file;
                int retu = recv( fd, &file, sizeof( file ), 0 );
                if ( retu < 0 ) {  //出错
                    cout << "thread " << fd << " error\n";
                    break;
                } else if ( !retu ) {  //关闭连接
                    cout << "thread " << fd << "exit\n";
                    close( fd );
                } else {  //正常接收，返回信息
                    /* 获取文件大小 */
                    struct stat st;  
                    memset(&st, 0, sizeof(st));  
                    char filename[100];
                    strcpy( filename, file.filename );
                    stat(filename, &st);  
                    int filesize = st.st_size;

                    file.info.size = filesize / file.info.num;

                    /* 发送文件 */
                    int j;
                    int file_fd = open( file.filename, O_RDONLY );
                    for( j = 0; j < file.info.num - 1; j++ ) {
                        file.info.i = j;  //发送当前为第几个部分
                        off_t offset = filesize / file.info.num * j;
                        lseek( file_fd, offset, SEEK_SET );
                        while( tell( file_fd ) < filesize / file.info.num * ( j+1 ) ) {
                        memset( file.info.buf, 0, BUFFER_SIZE );
                            memset( file.info.buf, 0, BUFFER_SIZE );                            
                            read( file_fd, file.info.buf, BUFFER_SIZE );
                            int t = send( fd, &file, sizeof( file ), 0 );
    printf("i = %d, n = %d, filesize = %d\n", file.info.i, file.info.num, filesize );
                            
    sleep( 1 );
                        }                        
                        
                    }

                    /* 发送最后一部分 */
                    file.info.i = j;  //发送当前为第几个部分
                    off_t offset = filesize / file.info.num * j;
                    lseek( file_fd, offset, SEEK_SET );
                    while( tell( file_fd ) < filesize ) {
                        memset( file.info.buf, 0, BUFFER_SIZE );
                        read( file_fd, file.info.buf, BUFFER_SIZE );
                        int t = send( fd, &file, sizeof( file ), 0 );
    sleep( 1 );
    printf("t_i = %d, n = %d, filesize = %d\n", file.info.i, file.info.num, filesize );
                    }  
                    close( fd );
                    return -1;
                 }
            }
        }
    }

    return 0;
}