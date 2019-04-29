#include "../include/func.h"
int cdFunc(char *username,char *path,char *homePath,char *Home)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);     //确保后续的 cd 操作是原子操作, 中途不会被打断
    char currentPath[1024]={0};
    //printf("current path0=%s\n",getcwd(NULL,0));
    if (!strcmp(path,"/"))
    {
        memset(currentPath,0,sizeof(currentPath));
        pthread_mutex_lock(&mutex);
        path_query(username,currentPath);
        chdir(currentPath);    //首先切换到当前用户所在的目录
        chdir(homePath);
        path_update(username,homePath);
        pthread_mutex_unlock(&mutex);
      //  printf("current path1=%s\n",getcwd(NULL,0));
        return 0;
    }
    memset(currentPath,0,sizeof(currentPath));
    pthread_mutex_lock(&mutex);
    path_query(username,currentPath);
    chdir(currentPath);    //首先切换到当前用户所在的目录
    chdir(path);
    path_update(username,getcwd(NULL,0));     // 更改数据库中当前用户所处的当前路径
    pthread_mutex_unlock(&mutex);
    //printf("current path2=%s\n",getcwd(NULL,0));
    if (!strcmp(getcwd(NULL,0),Home))       //确保当前账户处于自己的home 目录下面
    {  
        memset(currentPath,0,sizeof(currentPath));
        pthread_mutex_lock(&mutex);
        path_query(username,currentPath);
        chdir(currentPath);    //首先切换到当前用户所在的目录
        chdir(homePath);
        path_update(username,homePath);
        pthread_mutex_unlock(&mutex);
        printf("current path3=%s\n",getcwd(NULL,0));
    }
    return 0;
}


void displayFileTypel(mode_t st_mode,Train *t)
{
    switch(st_mode&0170000)			//此处我真的没看懂!!!
    {
    case S_IFLNK:
        sprintf(t->buf,"%s","l");
        break;
    case S_IFREG:
        sprintf(t->buf,"%s","-");
        break;
    case S_IFBLK:
        sprintf(t->buf,"%s","b");
        break;
    case S_IFDIR:
        sprintf(t->buf,"%s","d");
        break;
    case S_IFCHR:
        sprintf(t->buf,"%s","c");
        break;
    case S_IFIFO:
        sprintf(t->buf,"%s","p");
        break;
    case S_IFSOCK:
        sprintf(t->buf,"%s","s");
        break;
    }
}
int lsFunction(char *argv,int new_fd,char *user)
{
    DIR *dir;
    Train t;
    pthread_mutex_t mutex;          //确保对其他客户端的操作不会影响到本客户端
    pthread_mutex_init(&mutex,NULL);            
    pthread_mutex_lock(&mutex);
    char currentpath[1024]={0};
    path_query(user,currentpath);
  //  printf("current path=%s\n",currentpath);
    chdir(currentpath);
    dir=opendir(argv);
    if(NULL==dir)
    {
        perror("opendir");
        return -1;
    }
    int ret;
    struct stat buf;
    struct dirent *p;
    char path[1024];
    char *mon[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    while((p=readdir(dir))!=NULL)
    {
        if (!strcmp(p->d_name,".")||!strcmp(p->d_name,".."))
        {
            continue;
        }
        sprintf(path,"%s/%s",argv,p->d_name);
        ret=stat(path,&buf);
        if(-1==ret)
        {
            perror("stat");
            return -1;
        }
        struct tm *pT=gmtime(&buf.st_mtime);
        displayFileTypel(buf.st_mode,&t);
        sprintf(t.buf+1,"  %10ldB   %3s   %2d   %02d:%02d   %s",buf.st_size,mon[pT->tm_mon],pT->tm_mday,(8+pT->tm_hour),pT->tm_min,p->d_name);
        t.datalen=strlen(t.buf);
        ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
        if (-1==ret)
        {
            return -1;
        }
    }
    //发送结束标志，标志ls 指令的结束
    t.datalen=0;
    sendCycle(new_fd,(char*)&t,4);
    closedir(dir);
    pthread_mutex_unlock(&mutex);
    return 0;
}


int mkdirFunction(char *path,char* user)
{
    pthread_mutex_t mutex;          //确保对其他客户端的操作不会影响到本客户端
    pthread_mutex_init(&mutex,NULL);            
    pthread_mutex_lock(&mutex);
    char currentpath[1024]={0};
    path_query(user,currentpath);
    chdir(currentpath);
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    pthread_mutex_unlock(&mutex);
}
int pwdFunction(int new_fd,char *user)
{
    Train t;
    int ret;
    pthread_mutex_t mutex;      //设置线程锁的目的是确保其他线程切换目录对当前目录不会造成影响
    pthread_mutex_init(&mutex,NULL);
    char currentpath[1024]={0};
    pthread_mutex_lock(&mutex);
    path_query(user,currentpath);
    chdir(currentpath);
   // printf("pwd part,current path=%s\n",getcwd(NULL,0));
   // chdir(".");
   // printf("pwd part,current patht=%s\n",getcwd(NULL,0));
    sprintf(t.buf,"%s",getcwd(NULL,0));
    pthread_mutex_unlock(&mutex);
    t.datalen=strlen(t.buf);
    ret=sendCycle(new_fd,(char*)&t,4+t.datalen);     //发送当前所处路径给客户端
     if (-1==ret)
     {
         return -1;
     }
     //发送结束标志给客户端
     t.datalen=0;                       
     sendCycle(new_fd,(char*)&t,4);
    return 0;
}

int readConf(char *path,char *ip,char *port,int* threadNum,int* capacity)
{
    FILE *fp;
    fp=fopen(path,"rb");
    char buf[128]={0};
    if (NULL==fp)
    {
        perror("fopen");
    }
    fgets(buf,sizeof(buf),fp);      //读ip 地址
    buf[strlen(buf)-1]='\0';
    strcpy(ip,buf);
    
    memset(buf,0,sizeof(buf));
    fgets(buf,sizeof(buf),fp);      //读 port 端口号
    strcpy(port,buf);
   
    memset(buf,0,sizeof(buf));      //读取线程池中线程个数
    fgets(buf,sizeof(buf),fp);
    *threadNum=atoi(buf);
    
    memset(buf,0,sizeof(buf));      //读缓冲队列的长度
    fgets(buf,sizeof(buf),fp);
    *capacity=atoi(buf);
    return 0;
}


int recvFile(int socketFd)
{

    int ret,dataLen;
    char buf[1000]={0};
    //接收文件名
    recvCycle(socketFd,(char*)&dataLen,sizeof(int));
    recvCycle(socketFd,buf,dataLen);
    //接收文件大小
    off_t fileSize;
    recvCycle(socketFd,(char*)&dataLen,sizeof(int));
    recvCycle(socketFd,(char*)&fileSize,dataLen);
    //接文件内容
    int fd;
    fd=open(buf,O_CREAT|O_RDWR,0666);
    ERROR_CHECK(fd,-1,"open");
    while(1)
    {
        recvCycle(socketFd,(char*)&dataLen,sizeof(int));
        if(dataLen>0)
        {
            recvCycle(socketFd,buf,dataLen);
            write(fd,buf,dataLen);
        }
        else
        {
            break;
        }
    }
    close(fd);
}


int remove_dir(const char *dir)
{
    char cur_dir[] = ".";
    char up_dir[] = "..";
    char dir_name[1024];
    DIR *dirp;
    struct dirent *dp;
    struct stat dir_stat;

    // 参数传递进来的目录不存在，直接返回
    if ( 0 != access(dir, F_OK) ) 
    {
        return 0;
    }

    // 获取目录属性失败，返回错误
    if ( 0 > stat(dir, &dir_stat) ) 
    {
        perror("get directory stat error");
        return -1;
    }

    if ( S_ISREG(dir_stat.st_mode) ) 
    {  // 普通文件直接删除
        remove(dir);
    }
    else if ( S_ISDIR(dir_stat.st_mode) ) 
    {   // 目录文件，递归删除目录中内容
        dirp = opendir(dir);
        while ( (dp=readdir(dirp)) != NULL ) 
        {
            // 忽略 . 和 ..
            if ( (0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name)) ) 
            {
                continue;
            }

            sprintf(dir_name, "%s/%s", dir, dp->d_name);
            remove_dir(dir_name);   // 递归调用
        }
        closedir(dirp);
        rmdir(dir);     // 删除空目录
    } 
    else
    {
        perror("unknow file type!");    
    }
    return 0;
}


int tcpInit(int *sfd,char* ip,char *port)
{
	int socketFd=socket(AF_INET,SOCK_STREAM,0);
	ERROR_CHECK(socketFd,-1,"socket");
	struct sockaddr_in serAddr;
	memset(&serAddr,0,sizeof(serAddr));
	serAddr.sin_family=AF_INET;
	serAddr.sin_port=htons(atoi(port));//把端口转换为网络字节序
	serAddr.sin_addr.s_addr=inet_addr(ip);//把IP地址转换为网络字节序
	int ret;
	int reuse=1;
	ret=setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	ERROR_CHECK(ret,-1,"setsockopt");
	ret=bind(socketFd,(struct sockaddr*)&serAddr,sizeof(struct sockaddr));
	ERROR_CHECK(ret,-1,"bind");
	listen(socketFd,10);
	*sfd=socketFd;
}


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


int tranFile(int new_fd,char *fileName)
{
    int dataLen;
    off_t LocalFileSize=0;
    Train t;//定义发送数据的结构体
    int ret;
    int flag=2;       //用于记录待下载文件，是否需要断点重传
    //因为用于判断本地文件是否存在的库函数 access，成功返回0，失败返回-1,故flag=2
    recvCycle(new_fd,(char*)&flag,sizeof(flag));
    printf("flag=%d\n",flag);
    if (0==flag)
    {
        printf("File exits\n");         //接收本地已经存在部分的大小
        recvCycle(new_fd,(char*)&LocalFileSize,sizeof(LocalFileSize));
        printf("LocalFileSize=%ld\n",LocalFileSize);
    }
    //发送文件名给客户端
     t.datalen=strlen(fileName);
     strcpy(t.buf,fileName);
     ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
     if(-1==ret)
     {
     	return -1;
     }
    int fd=open(fileName,O_RDONLY);
    ERROR_CHECK(fd,-1,"open");
    //发送文件大小给客户端
    struct stat buf;
    off_t tmp;
    fstat(fd,&buf);
    t.datalen=sizeof(buf.st_size);
    if (0==flag)
    {
        tmp=buf.st_size-LocalFileSize;
       ret= lseek(fd,LocalFileSize,SEEK_SET);
       printf("ret=%d\n",ret);
    }
    else
    {
        tmp=buf.st_size;
    }
    printf("tmp=%ld\n",tmp);
    if (0==tmp)
    {   
        tmp=buf.st_size;        //将 tmp 重新设置为文件大小，后续的操作重新下载文件
    }
    memcpy(t.buf,&tmp,sizeof(buf.st_size));
    ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
    if(-1==ret)
    {
        return -1;
    }
    off_t ptrPos=0;//记录目前mmap指针的偏移
    char *pMap=mmap(NULL,tmp,PROT_READ,MAP_SHARED,fd,0);
    ERROR_CHECK(pMap,(char*)-1,"mmap");	
    //发送文件内容给客户端
    // 小于100M 普通循环发送
     char buffer[1000];
    if (tmp<=(long)100*1024*1024)
    {
        printf("fileSize less than 100M\n");
        while ((dataLen=read(fd,buffer,sizeof(buffer))))
        {
            ret=sendCycle(new_fd,(char*)&dataLen,sizeof(int));
            if (-1==ret)
            {
                close(fd);
                return -1;
            }
            ret=sendCycle(new_fd,buffer,dataLen);
            if (-1==ret)
            {
                close(fd);
                return -1;
            }
        }
        dataLen=0;
        ret=sendCycle(new_fd,(char*)&dataLen,sizeof(int));
        if (-1==ret)
        {
            close(fd);
            return -1; 
        }
    }
    //文件大于100M用 mmap 方式发送
    else {
        printf("fileSize more than 100M\n");
       if(tmp>1000)
          {
              while(ptrPos+1000<tmp)
              {
                  memcpy(t.buf,pMap+ptrPos,sizeof(t.buf));
                  t.datalen=1000;
                  ptrPos=ptrPos+1000;
                  ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
                  if(-1==ret)
                  {
                      return -1;
                  }
              }
              t.datalen=tmp-ptrPos;
          }else{
              t.datalen=tmp;
          }
          //最后一个发送内容的列车
          memcpy(t.buf,pMap+ptrPos,t.datalen);
          ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
          if(-1==ret)
          {
              return -1;
          }
          munmap(pMap,tmp);       //解除 mmap 映射
          t.datalen=0;
          sendCycle(new_fd,(char*)&t,4);//发送文件发送结束标志
    }
    return 0;
}
