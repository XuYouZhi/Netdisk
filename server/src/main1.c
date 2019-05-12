#include "../include/func.h"
#include "../include/factory.h"
#include "../include/work_que.h"
int main(int argc,char *argv[])
{
    int ret,retVal;
    ARGS_CHECK(argc,2);                  
    Factory_t f;
    int threadNum;
    int capacity;
    char funcSelect;         //瞬时响应命令和上传和下载命令分别对应这s和l
    char ip[128]={0},port[10]={0};
    readConf(argv[1],ip,port,&threadNum,&capacity);      //读取配置文件中的配置文件中的IP地址和端口号
    printf("threadNum=%d\n",threadNum);  
    printf("capacity=%d\n",capacity);
    struct sockaddr_in clientAddr;
    int socketFd,new_fd[capacity];     // capacity为服务器端创建的线程数，new_fd 数组为可以监控的new_fd 数
    tcpInit(&socketFd,ip,port);
    factoryInit(&f,threadNum,capacity);
    factoryStart(&f);
    int epfd=epoll_create(1);       //创建一个 epoll 句柄
    printf("epfd=%d\n",epfd);
    struct epoll_event event,evs[10];            // 目前只监控 socketFd 一个描述符，所以定义的数组长度为1
    event.events=EPOLLIN|EPOLLET;           //EPOLLIN 表示对应的文件描述符可读(包括对端 SOCKET 正常 关闭)
    event.data.fd=socketFd;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,socketFd,&event);
    printf("epoll_ctl ret=%d\n",ret);
    // 第一个参数是 epoll_create()的返回值
    //  第二个参数表示动作，用宏来表示，EPOLL_CTL_ADD， 注册新的 fd 到 epfd 中
    //  第三个参数是需要监听的 fd
    //   第四个参数是告诉内核需要监听什么事
     int fdReadNum,i;
     int j=0;
    pQue_t pq=&f.que;
    while (1)
    {                                           //暂时 epoll 只负责监听 socketFd
        fdReadNum=epoll_wait(epfd,evs,10,-1);       //等待事件的产生，该函数返回需要处理的事情数目
       printf("fdReadNum=%d\n",fdReadNum);
        for (i=0;i<fdReadNum;++i)
        {
            if (socketFd==evs[i].data.fd)
            {
                 memset(&clientAddr,0,sizeof(clientAddr));
                 socklen_t sockLen=sizeof(struct sockaddr);
                 new_fd[j]=accept(socketFd,(struct sockaddr*)&clientAddr,&sockLen);
                 printf("client ip=%s,port=%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
                 //根据接收到的funcSelect来区分是瞬时响应命令还是上传和下载命令
                 recvCycle(new_fd[j],(char*)&funcSelect,sizeof(char));
                 printf("funcSelect=%c\n",funcSelect);
                 if (funcSelect=='l')
                 {
                     //上传或下载命令
                     printf("long time part\n");
                     pNode_t pnew=(pNode_t)calloc(1,sizeof(Node_t));
                     pnew->new_fd=new_fd[j];
                     pthread_mutex_lock(&pq->mutex);
                     queInsert(pq,pnew);
                     pthread_mutex_unlock(&pq->mutex);
                     pthread_cond_signal(&f.cond);
                     ++j;               //出队的时候，j 需要-1, 想想怎么实现
                 }
                 else if (funcSelect=='s')
                 {
                     printf("part 1,j=%d\n",j);
                        event.events=EPOLLIN|EPOLLET;
                        event.data.fd=new_fd[j];
                        ret=epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd[j],&event);
                        printf("part 1,epoll_ctl ret=%d\n",ret);
                        Function1(new_fd[j]);
                       // if (-1==retVal)
                       // {
                       //     //将new_fd[j] 从监控序列中去除
                       //     ret=epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd[j],&event);
                       //     printf("part 1,epoll_ctl ret=%d\n",ret);
                       //     --j;
                       //     close(new_fd[j]);
                       // }
                        ++j;
                 }

            }
            for (int k=0;k<capacity;++k)
            {
                 if (new_fd[k]==evs[i].data.fd)
                 {
                     recvCycle(new_fd[k],(char*)&funcSelect,sizeof(char));
                     printf("funcSelect=%c\n",funcSelect);
                    // if (funcSelect=='l')        //Function1='l'说明这是 puts 或者 gets 命令
                    // {
                    //      //上传或下载命令
                    //      pNode_t pnew=(pNode_t)calloc(1,sizeof(Node_t));
                    //      pnew->new_fd=new_fd[k];
                    //      pthread_mutex_lock(&pq->mutex);
                    //      queInsert(pq,pnew);
                    //      pthread_mutex_unlock(&pq->mutex);
                    //      pthread_cond_signal(&f.cond);


                    // }
                     if (funcSelect=='s')
                     {
                         printf("part 2,j=%d\n",j);
                          retVal=Function1(new_fd[k]);
                          if (-1==retVal)
                          {
                              // 将new_fd[j]从 epoll 监控序列中去除
                               ret=epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd[k],&event);
                               printf("part 2,epoll_ctl ret=%d\n",ret);
                               --j;
                               printf("now j=%d\n",j);
                               close(new_fd[k]);
                          }
                     }
                 }

            }
       }
    }
    return 0;
}
