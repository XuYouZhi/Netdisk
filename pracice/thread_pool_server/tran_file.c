#include "tranfile.h"

int tranFile(int new_fd)
{
	train t;//定义发送数据的结构体
	int ret;
	t.dataLen=strlen(FILENAME);
	strcpy(t.buf,FILENAME);
	//发送文件名给客户端
	ret=sendCycle(new_fd,(char*)&t,4+t.dataLen);
	if(-1==ret)
	{
		return -1;
	}
	int fd=open(FILENAME,O_RDONLY);
	//发送文件大小给客户端
	struct stat buf;
	fstat(fd,&buf);
    printf("fileSize=%ld\n",buf.st_size);
	t.dataLen=sizeof(buf.st_size);
	memcpy(t.buf,&buf.st_size,sizeof(buf.st_size));
	ret=sendCycle(new_fd,(char*)&t,4+t.dataLen);
	if(-1==ret)
	{
		return -1;
	}
	off_t ptrPos=0;//记录目前mmap指针的偏移
	char *pMap=mmap(NULL,buf.st_size,PROT_READ,MAP_SHARED,fd,0);
	ERROR_CHECK(pMap,(char*)-1,"mmap");	
	//发送文件内容给客户端
	if(buf.st_size>1000)
	{
		while(ptrPos+1000<buf.st_size)
		{
			memcpy(t.buf,pMap+ptrPos,sizeof(t.buf));
			t.dataLen=1000;
			ptrPos=ptrPos+1000;
			ret=sendCycle(new_fd,(char*)&t,4+t.dataLen);
			if(-1==ret)
			{
				return -1;
			}
		}
		t.dataLen=buf.st_size-ptrPos;
	}else{
		t.dataLen=buf.st_size;
	}
	//最后一个发送内容的列车
	memcpy(t.buf,pMap+ptrPos,t.dataLen);
	ret=sendCycle(new_fd,(char*)&t,4+t.dataLen);
	if(-1==ret)
	{
		return -1;
	}
	t.dataLen=0;
	sendCycle(new_fd,(char*)&t,4);//发送文件发送结束标志
	return 0;
}

