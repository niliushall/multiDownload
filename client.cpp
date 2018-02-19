#include "server.h"

int main( int argc, char * argv[] ) {
    if ( argc < 4 ) {
        cout << "usage: ./client  ip_num  filename  thread_num\n";
        return -1;
    }

    const char * ip = argv[1];
    const int thread_num = atoi( argv[3] );

    int file_fd = open(argv[2], O_CREAT|O_TRUNC|O_RDWR, S_IRWXU);  //新建文件
    if ( file_fd < 0 ) {
        err( __LINE__ );
    }
    close( file_fd );

    /* epoll */
    epoll_event events[ MAX_EVENT_NUMBER ];  //内核事件表信息
    int epoll_fd = epoll_create( thread_num );
    if ( epoll_fd < 0 ) {
        err( __LINE__ );
    }

    myDownload file[ thread_num ];
    pthread_t thread[ thread_num ];  //所有线程号

    /* 创建 MAX_THREAD_NUM 个线程*/
    for( int i = 0; i < thread_num; i++ ) {
        /* 初始化信息 */
        file[i].address.sin_family = AF_INET;
        file[i].address.sin_port = htons( PORT );
        inet_pton( AF_INET, ip, &file[i].address.sin_addr );
        file[i].info.filename = argv[2];
        file[i].info.num = thread_num;
        file[i].info.i = i;
        
        pthread_create( &thread[i], NULL, myDownload::download, NULL );
    }

    for( int i = 0; i < thread_num; i++ ) {
        pthread_join( thread[i], NULL );
    }

    cout << "download file " << argv[2] << " success\n";

    return 0;
}