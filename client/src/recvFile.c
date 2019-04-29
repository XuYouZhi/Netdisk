#include "../include/func.h"
#include "../include/progress.h"
int recvFile(int socketFd,int flag)
{
    int ret,dataLen;
    char buf[1000]={0};             //将所有用户下载的文件放在此路径下
    sprintf(buf,"%s/%s",PATH,"UpAndDownLoad");
    chdir(buf);
    //接收文件名
    memset(buf,0,sizeof(buf));
    recvCycle(socketFd,(char*)&dataLen,sizeof(int));
    recvCycle(socketFd,buf,dataLen);
    off_t downLoadSize,fileSize,beforeSize,fileSlice;
    //接收文件大小
    recvCycle(socketFd,(char*)&dataLen,sizeof(int));
    recvCycle(socketFd,(char*)&fileSize,dataLen);
    printf("fileSize=%ld\n",fileSize);
    //接文件内容
    int fd;
    fd=open(buf,O_WRONLY|O_CREAT,0666);
    ERROR_CHECK(fd,-1,"open");
    if (0==flag)
    {
       ret= lseek(fd,0,SEEK_END);
       printf("ret=%d\n",ret);
    }
    fileSlice=fileSize/50;
    beforeSize=downLoadSize=0;
    progress_t bar;
    int i=0;
    progress_init(&bar, "",50, PROGRESS_CHR_STYLE); 
    while(1)
    {
        recvCycle(socketFd,(char*)&dataLen,sizeof(int));
        if(dataLen>0)
        {
            recvCycle(socketFd,buf,dataLen);
            downLoadSize+=dataLen;
            write(fd,buf,dataLen);
            if(downLoadSize-beforeSize>=fileSlice)
            {
                progress_show(&bar,i/50.0f);
                ++i;
                beforeSize=downLoadSize;
            }
        }
        else
        {
            progress_show(&bar,50/50.0f);
            printf("\n+-Done\n");
            break;
        }
    }
    progress_destroy(&bar);
    close(fd);
}
