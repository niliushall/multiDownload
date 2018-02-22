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
    addfd( epoll_fd, sock_fd );

    while( true ) {
        ret = epoll_wait( epoll_fd, events, MAX_EVENT_NUMBER, -1 );
// cout << "ret = " << ret << endl;
        if ( ret < 0 ) {
            cout << "epoll_wait error\n";
            return -1;
        }

        for( int i = 0; i < ret; i++ ) {
            int fd = events[i].data.fd;
            if ( fd == sock_fd ) {
                struct sockaddr_in client;
                socklen_t client_length = sizeof( client );
                int conn_fd = accept( sock_fd, ( struct sockaddr * )&client,
                                        &client_length );
// cout << "accept = " << accept << endl;
                if ( conn_fd <= 0 ) {
                    cout << "accept error\n";
                    return -1;
                }
                addfd( epoll_fd, conn_fd );
myDownload te;
strcpy(te.info.buf , "good");
cout << "fd = " << fd << endl;
int s = send( fd, &te, sizeof(te), 0 );
cout << "jinjin\n";
cout << "s = " << s << endl;

            } else if ( events[i].events & EPOLLIN ) {
                myDownload tempfile;
                int retu = recv( fd, &tempfile, sizeof( tempfile ), 0 );
cout << "retu = " << retu << endl;
                if ( retu < 0 ) {
                    cout << "thread " << fd << " error\n";
                    break;
                } else if ( !retu ) {
                    cout << "thread " << fd << "exit\n";
                    close( fd );
                } else {
                    struct stat st;  
                    memset(&st, 0, sizeof(st));  
                    char filename[100];
                    strcpy( filename, tempfile.filename.c_str() );
                    stat(filename, &st);  
                    int filesize = st.st_size;  

                    FILE *fp = fopen( filename, "r" );

                    for( int i = 0; i < tempfile.info.num; i++ ) {
                        tempfile.info.i = i;
                        fseek( fp, filesize / tempfile.info.num * i, SEEK_SET );
                        while( ftell( fp ) < ( filesize / tempfile.info.num * ( i + 1 ) ) ) {
                            fread( tempfile.info.buf, BUFFER_SIZE, 1, fp );
                            send( fd, &tempfile, sizeof( tempfile ), 0 );
                        }
                    }
                }
            }
        }
    }

    cout << "send file finished\n";
    close( sock_fd );
    
    return 0;
}