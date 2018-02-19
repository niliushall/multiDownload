#include "server.h"

int main(int argc, char * argv) {
    if ( argc < 3 ) {
        cout << "usage: ./client  ip_num  filename\n";
        return -1;
    }

    const char * ip = argv[1];

    int file_fd = open(argv[2], O_CREAT|O_TRUNC|O_RDWR, S_IRWXU);  //新建文件
    if ( file_fd < 0 ) {
        err( __LINE__ );
    }
    close( file_fd );

    /* epoll */
    epoll_event events[MAX_EVENT_NUMBER];  //内核事件表信息
    int epoll_fd = epoll_create( 5 );
    if ( epoll_fd < 0 ) {
        err( __LINE__ );
    }

    myDownload file[MAX_THREAD_NUM];
    pthread_t thread[MAX_THREAD_NUM];  //所有线程号

    /* 创建 MAX_THREAD_NUM 个线程*/
    for( int i = 0; i < MAX_THREAD_NUM; i++ ) {
        /* 初始化信息 */
        file[i].filename = argv[1];
        file[i].address.sin_family = AF_INET;
        file[i].address.sin_port = htons( PORT );
        inet.pton( AF_INET, ip, &file[i].address.sin_addr );
        file[i].filename = argv[2];
        file[i].num = i;
        
        pthread_create( &thread[i], NULL, file[i].download, NULL );
    }

    for( int i = 0; i < MAX_THREAD_NUM; i++ ) {
        pthread_join( thread[i], NULL );
    }

    cout << "download file " << argv[2] << " success\n";

    return 0;
}