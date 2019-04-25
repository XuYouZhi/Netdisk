#include "../include/func.h"
#include "../include/progress.h"
int tranFile(int socketFd,char *fileName)
{
    char *path;
    path=getcwd(NULL,0);
    char buffer[1000]={0};                 
    //所有用户待上传的文件都存于此路径下
    sprintf(buffer,"%s/%s",path,"UpAndDownLoad");
    chdir(buffer);

    //按上传的文件大小方式打印进度条
    Train t;//定义发送数据的结构体
    int ret;
    t.datalen=strlen(fileName);
    strcpy(t.buf,fileName);
    //发送文件名服务器
    ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
    if(-1==ret)
    {
        return -1;
    }
    int fd=open(fileName,O_RDONLY);
    ERROR_CHECK(fd,-1,"open");
    //发送文件大小给服务器
    struct stat buf;
    fstat(fd,&buf);
    t.datalen=sizeof(buf.st_size);
    memcpy(t.buf,&buf.st_size,sizeof(buf.st_size));
    ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
    if(-1==ret)
    {
        return -1;
    }
    if (buf.st_size>(long)100*1024*1024)        //文件大于100M,采用 mmap 的方式进行传输
    {
         off_t upLoadSize,beforeSize,fileSlice;
         off_t ptrPos=0;//记录目前mmap指针的偏移
         char *pMap=mmap(NULL,buf.st_size,PROT_READ,MAP_SHARED,fd,0);
         ERROR_CHECK(pMap,(char*)-1,"mmap");	
         //发送文件内容给客户端
         beforeSize=upLoadSize=0;
         fileSlice=buf.st_size/50;
         progress_t bar;
         int i=0;
         progress_init(&bar, "",50, PROGRESS_CHR_STYLE);
         if(buf.st_size>1000)
         {
             while(ptrPos+1000<buf.st_size)
             {
                 memcpy(t.buf,pMap+ptrPos,sizeof(t.buf));
                 t.datalen=1000;
                 ptrPos=ptrPos+1000;
                 ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
                 if(-1==ret)
                 {
                     return -1;
                 }
                 if (ptrPos-beforeSize>=fileSlice)
                 {
                     //     printf("\r%5.2f%s",(double)ptrPos/buf.st_size*100,"%");
                     //   fflush(stdout);
                     progress_show(&bar,i/50.0f);
                     ++i;
                     beforeSize=ptrPos;
                 }
             }
             t.datalen=buf.st_size-ptrPos;
         }else{
             t.datalen=buf.st_size;
         }
         //最后一个发送内容的列车
         memcpy(t.buf,pMap+ptrPos,t.datalen);
         ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
         if(-1==ret)
         {
             return -1;
         }
         munmap(pMap,buf.st_size);
         //printf("\r100.00%s\n","%");
         progress_show(&bar,50/50.0f);
         printf("\n+-Done\n");
         t.datalen=0;
         sendCycle(socketFd,(char*)&t,4);//发送文件发送结束标志
          progress_destroy(&bar);
         close(fd);
    }
     else           //文件小于100M，则采用循环发送的方式
     {
        printf("fileSize less than 100M\n");
        progress_t bar;
        int i=0;
        progress_init(&bar, "",50, PROGRESS_CHR_STYLE);
        off_t currentSize=0,beforeSize=0;
        off_t fileSlice=buf.st_size/50;
        int dataLen=0;
        while ((dataLen=read(fd,buffer,sizeof(buffer))))
        {
            ret=sendCycle(socketFd,(char*)&dataLen,sizeof(int));
            if (-1==ret)
            {
                close(fd);
                return -1;
            }
            if (dataLen>0)
            {
                ret=sendCycle(socketFd,buffer,dataLen);
                if (-1==ret)
                {
                    close(fd);
                    return -1;
                }
                currentSize+=dataLen;
                if (currentSize-beforeSize>=fileSlice)
                {
                    progress_show(&bar,i/50.0f);
                    ++i;
                    beforeSize=currentSize;
                }
            }
        }
        progress_show(&bar,50/50.0f);
        printf("\n+-Done\n");
        dataLen=0;
        ret=sendCycle(socketFd,(char*)&dataLen,sizeof(int));
        if (-1==ret)
        {
            close(fd);
            return -1;
        }
        progress_destroy(&bar);
        close(fd);
     }
    return 0;
}
