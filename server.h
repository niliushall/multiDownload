#include <iostream>
#include <string>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
using namespace std;

const int BUFFER_SIZE = 1024;
const int MAX_EVENT_NUMBER = 1024;
const int PORT = 6666;

struct file_info {
    string filename;
    int num;  //总线程数
    int i;  //标记当前为第几个线程
    int start;  //起始位置
    int size;  //需要数据大小
};

class myDownload {
public:
    int sock_fd;
    int epoll_fd;
    file_info info;
    // string filename;
    struct sockaddr_in address;

    myDownload();
    // ~myDownload() {};

    int setnonblocking( int );  //将fd设为非阻塞
    int addfd( int, int );  //添加epoll监听socket
    static void * download( void * );  //客户端下载文件的工作函数
    void * sendfile();
};

void err( int );