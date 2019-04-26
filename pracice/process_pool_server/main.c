#include "func.h"

int exitFds[2];//用来退出的管道
void sigExitFunc(int signum)
{
	write(exitFds[1],&signum,1);
}

int main(int argc,char *argv[])
{
	args_check(argc,4);
	int processNum=atoi(argv[3]);//要创建的进程数目
	Process_Data *p=(Process_Data *)calloc(processNum,sizeof(Process_Data));//p指向我们的主要数据结构
	makeChild(p,processNum);//创建子进程并初始化数据结构
	pipe(exitFds);
	signal(SIGUSR1,sigExitFunc);
	int epfd=epoll_create(1);
	struct epoll_event event,*evs;
	evs=(struct epoll_event*)calloc(processNum+2,sizeof(struct epoll_event));
	int i,ret;
	for(i=0;i<processNum;i++)
	{
		event.events=EPOLLIN;
		event.data.fd=p[i].fd;
		ret=epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fd,&event);
		ERROR_CHECK(ret,-1,"epoll_ctl");
	}
	int socketFd;
	tcpInit(&socketFd,argv[1],argv[2]);//封装socket,bind,listen
	int readyFdNum,new_fd;
	int j;
	event.events=EPOLLIN;
	event.data.fd=socketFd;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,socketFd,&event);
	ERROR_CHECK(ret,-1,"epoll_ctl1");
	//用来实现退出机制，异步拉同步
	event.data.fd=exitFds[0];
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,exitFds[0],&event);
	ERROR_CHECK(ret,-1,"epoll_ctl1");
	pid_t pid;
	while(1)
	{
		readyFdNum=epoll_wait(epfd,evs,processNum+2,-1);
		for(i=0;i<readyFdNum;i++)
		{
			if(evs[i].data.fd==socketFd)
			{
				new_fd=accept(socketFd,NULL,NULL);
				for(j=0;j<processNum;j++)//遍历找到非忙碌的子进程，然后把new_fd给它
				{
					if(0==p[j].busy)
					{
						sendFd(p[j].fd,new_fd,0);
						p[j].busy=1;//设置子进程为忙碌
						printf("%d is busy\n",p[j].pid);
						break;
					}
				}
				close(new_fd);
			}
			//退出机制
			if(evs[i].data.fd==exitFds[0])
			{
				for(j=0;j<processNum;j++)
				{
			//	kill(p[j].pid,9);//子进程业务不重要
					//要让子进程同步退出
				sendFd(p[j].fd,0,1);
				}
				for(j=0;j<processNum;j++)
				{
					pid=wait(NULL);
					printf("%d is exit\n",pid);
				}
				exit(0);
			}
			for(j=0;j<processNum;j++)
			{
				if(evs[i].data.fd==p[j].fd)
				{
					p[j].busy=0;
					read(p[j].fd,&ret,1);
					printf("%d is not busy\n",p[j].pid);
					break;
				}
			}
		}
	}
}
