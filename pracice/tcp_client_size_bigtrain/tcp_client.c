#include "func.h"

int recvCycle(int,char*,int);
int main(int argc,char* argv[])
{
	args_check(argc,3);
	int socketFd=socket(AF_INET,SOCK_STREAM,0);
	ERROR_CHECK(socketFd,-1,"socket");
	struct sockaddr_in serAddr;
	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family=AF_INET;
	serAddr.sin_port=htons(atoi(argv[2]));//把端口转换为网络字节序
	serAddr.sin_addr.s_addr=inet_addr(argv[1]);//把IP地址转换为网络字节序
	int ret;
	ret=connect(socketFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
	ERROR_CHECK(ret,-1,"connect");
	char buf[1000]={0};
	int dataLen;
	//接收文件名
	recvCycle(socketFd,(char*)&dataLen,sizeof(int));
	recvCycle(socketFd,buf,dataLen);
	//接文件大小
	off_t downLoadSize,fileSize,beforeSize,fileSlice;
	recvCycle(socketFd,(char*)&dataLen,sizeof(int));
	recvCycle(socketFd,(char*)&fileSize,dataLen);
	printf("fileSize=%ld\n",fileSize);
	//接文件内容
	int fd;
	fd=open(buf,O_CREAT|O_RDWR,0666);
	ERROR_CHECK(fd,-1,"open");
	fileSlice=fileSize/100;
	beforeSize=downLoadSize=0;
	struct timeval start,end;
	gettimeofday(&start,NULL);
	ftruncate(fd,fileSize);//做一个同样大小的文件
	char *pMap=mmap(NULL,fileSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	//接文件内容
	ret=recvCycle(socketFd,pMap,fileSize);
	if(-1==ret)
	{
		return -1;
	}
	munmap(pMap,fileSize);
	gettimeofday(&end,NULL);
	//接结束符
	recvCycle(socketFd,(char*)&dataLen,sizeof(int));
	printf("use time=%ld\n",(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec);
	close(fd);
	close(socketFd);
}
