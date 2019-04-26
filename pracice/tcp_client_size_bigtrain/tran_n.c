#include "func.h"
//循环发送
int sendCycle(int fd,char* p,int sendLen)
{
	int total=0;
	int ret;
	while(total<sendLen)
	{
		ret=send(fd,p+total,sendLen-total,0);
		total=total+ret;
	}
	return 0;
}
//循环接收,退出机制
int recvCycle(int fd,char* p,int recvLen)
{
	int total=0;
	int ret;
	while(total<recvLen)
	{
		ret=recv(fd,p+total,recvLen-total,0);
		if(0==ret)
		{
			return -1;
		}
		total=total+ret;
		//只跟大火车的进度条配对
    	printf("\r%5.2f%s",(double)total/recvLen*100,"%");
    	fflush(stdout);
	}
	return 0;
}
