#include "tranfile.h"
//循环发送,退出机制
int sendCycle(int fd,char* p,int sendLen)
{
	int total=0;
	int ret;
	while(total<sendLen)
	{
		ret=send(fd,p+total,sendLen-total,0);
		if(-1==ret)
		{
			return -1;
		}
		total=total+ret;
	}
	return 0;
}
//循环接收
int recvCycle(int fd,char* p,int recvLen)
{
	int total=0;
	int ret;
	while(total<recvLen)
	{
		ret=recv(fd,p+total,recvLen-total,0);
		total=total+ret;
	}
	return 0;
}
