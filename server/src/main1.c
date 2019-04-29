#include "../include/func.h"
#include "../include/factory.h"
#include "../include/work_que.h"
int main(int argc,char *argv[])
{
    int ret;
    ARGS_CHECK(argc,2);                  
    Factory_t f;
    int threadNum;
    int capacity;
    
    int socketFd,new_fd;
    char ip[128]={0},port[10]={0};
    readConf(argv[1],ip,port,&threadNum,&capacity);      //读取配置文件中的配置文件中的IP地址和端口号
    printf("threadNum=%d\n",threadNum);  
    printf("capacity=%d\n",capacity);
    struct sockaddr_in clientAddr;
    tcpInit(&socketFd,ip,port);
    factoryInit(&f,threadNum,capacity);
    factoryStart(&f);
    int epfd=epoll_create(1);       //创建一个 epoll 句柄
    printf("epfd=%d\n",epfd);
    struct epoll_event event,evs[1];            // 目前只监控 socketFd 一个描述符，所以定义的数组长度为1
    event.events=EPOLLIN|EPOLLET;           //EPOLLIN 表示对应的文件描述符可读(包括对端 SOCKET 正常 关闭)
    event.data.fd=socketFd;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,socketFd,&event);
    printf("epoll_ctl ret=%d\n",ret);
    // 第一个参数是 epoll_create()的返回值
    //  第二个参数表示动作，用宏来表示，EPOLL_CTL_ADD， 注册新的 fd 到 epfd 中
    //  第三个参数是需要监听的 fd
    //   第四个参数是告诉内核需要监听什么事
     int fdReadNum,i;
    pQue_t pq=&f.que;
    while (1)
    {                                           //暂时 epoll 只负责监听 socketFd
        fdReadNum=epoll_wait(epfd,evs,1,-1);       //等待事件的产生，该函数返回需要处理的事情数目
       printf("fdReadNum=%d\n",fdReadNum);
        for (i=0;i<fdReadNum;++i)
        {
            if (socketFd==evs[i].data.fd)
            {
                 memset(&clientAddr,0,sizeof(clientAddr));
                 socklen_t sockLen=sizeof(struct sockaddr);
                 new_fd=accept(socketFd,(struct sockaddr*)&clientAddr,&sockLen);
                 printf("client ip=%s,port=%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
                 pNode_t pnew=(pNode_t)calloc(1,sizeof(Node_t));
                 pnew->new_fd=new_fd;
                 pthread_mutex_lock(&pq->mutex);
                 queInsert(pq,pnew);
                 pthread_mutex_unlock(&pq->mutex);
                 pthread_cond_signal(&f.cond);
            }
        }
    }
    return 0;
}
