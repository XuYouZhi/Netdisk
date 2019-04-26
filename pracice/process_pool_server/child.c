#include "func.h"
int makeChild(Process_Data *p,int processNum)
{
	int i,ret;
	pid_t pid;
	int fds[2];//管道
	for(i=0;i<processNum;i++)
	{
		ret=socketpair(AF_LOCAL,SOCK_STREAM,0,fds);//放到for里边
		ERROR_CHECK(ret,-1,"socketpair");
		//pipe(fds);
		pid=fork();
		if(0==pid)
		{
			close(fds[1]);
			childHandle(fds[0]);
		}
		//父进程区域,数据初始化完毕
		close(fds[0]);
		p[i].pid=pid;
		p[i].fd=fds[1];
	}
}

void childHandle(int fd)
{
	int new_fd;
	int exitFlag;
	while(1)
	{
		recvFd(fd,&new_fd,&exitFlag);//接收父进程给的任务
		if(exitFlag)
		{
			exit(0);
		}
		tranFile(new_fd);//发送文件给客户端
		close(new_fd);//引用计数为0，内核才会关闭连接
		write(fd,&new_fd,1);//发送通知告诉父进程完成任务
	}
}
