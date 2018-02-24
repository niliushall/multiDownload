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

                    /* 发送文件 */
                    int j;
                    int file_fd = open( file.filename, O_RDONLY );
                    for( j = 0; j < file.info.num - 1; j++ ) {
                        send( fd, &j, sizeof( j ), 0 );  //发送当前为第几个部分
                        off_t offset = filesize / file.info.num * j;
                        int ret = sendfile( fd, file_fd, &offset, filesize / file.info.num );
                        sleep(1);
    printf("size=%d, send=%d %d\n", filesize, ret, filesize / file.info.num);
                    }

                    /* 发送最后一部分 */
                    int s = send( fd, &j, sizeof( j ), 0 );  //发送当前为第几个部分
    printf("s = %d\n", s);
                    off_t offset = filesize / file.info.num * j;
                    int ret = sendfile( fd, file_fd, &offset,
                                filesize - filesize / file.info.num * (j-1) );
    printf("size=%d, send=%d  %d\n", filesize, ret,filesize / file.info.num * j);
                    close( fd );
                    return -1;

                    // printf("buffer=%s=\n",buf);
                    // char buf[1024] = "huichuan";
                    // int s = send(fd, buf,9,0);
                    // printf("send=%d\n",s);

                 }
            }
        }
    }

    // cout << "send file finished\n";
    return 0;
}