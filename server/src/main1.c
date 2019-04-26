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

    pQue_t pq=&f.que;
    while (1)
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
    return 0;
}
