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
    //printf("threadNum=%d\n",threadNum);  
    //printf("capacity=%d\n",capacity);
    
    //factoryInit(&f,threadNum,capacity);       //此处暂时注释掉
    //factoryStart(&f);
    struct sockaddr_in clientAddr;
    tcpInit(&socketFd,ip,port);
    fd_set rdset;
    fd_set realset;
    FD_ZERO(&realset);
    FD_SET(socketFd,&realset);
    while (1)
    {
      memcpy(&rdset,&realset,sizeof(rdset));
      ret=select(13,&rdset,NULL,NULL,NULL);
      if (ret>0)
      {
         if (FD_ISSET(socketFd,&rdset))
         {
             memset(&clientAddr,0,sizeof(clientAddr));
             socklen_t sockLen=sizeof(struct sockaddr);
             new_fd=accept(socketFd,(struct sockaddr*)&clientAddr,&sockLen);
             printf("client ip=%s,port=%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
             FD_SET(new_fd,&realset); 
         }
         if (FD_ISSET(new_fd,&rdset))
         {
             char comand[20]={0};             //用于接收命令
             char buf[1000]={0};
             int datalen;
             int  ret;
             char user[30]={0};
             char path[1024]={0};            
             char home[1000]={0};
        switchAcc:
           //printf("before logFunction\n");
             memset(user,0,sizeof(user));
             logFunction(new_fd,user);
            //printf("user=%s\n",user);
            //printf("After logFunction\n");
         //memset(buf,0,sizeof(buf));
         //recvCycle(new_fd,(char*)&datalen,sizeof(int));
         //recvCycle(new_fd,buf,datalen);
             memset(path,0,sizeof(path));
             strcpy(home,BASE_PATH);
             strcat(home,"/home");
             printf("home=%s\n",home);
             //char *home="/home/xuyouzhi/Netv1.2/server/home";    //具体程序放的位置同，得手动修改,此处是程序的隐含 bug，最好修正成自动获取
     // memset(home,0,sizeof(home));
      //sprintf(home,"%s/..",getcwd(NULL,0));
             sprintf(path,"%s/%s",home,user);       //将登录用户锁定在自己的 home 目录里面，这点极端重要，不然所有的其他命令都会出问题
             while (1)
             {
                 memset(comand,0,sizeof(comand));            //读取客户端发送过来的命令
                 memset(buf,0,sizeof(buf));                  //buf 清空，用于接收文件名或者路径名
                 printf("new_fd=%d\n",new_fd);
                 recvCycle(new_fd,(char*)&datalen,sizeof(int));
                 printf("datalen=%d\n",datalen);
                 recvCycle(new_fd,comand,datalen);
                 printf("comand=%s\n",comand);
                 if (!strcmp(comand,"quit"))
                 {
                     printf("byebye\n");
                     FD_CLR(new_fd,&realset);
                     close(new_fd);
                     break;
                 }
                 else if (!strcmp(comand,"ls"))
                 {
                     recvCycle(new_fd,(char*)&datalen,sizeof(int));
                     recvCycle(new_fd,buf,datalen);
                     lsFunction(buf,new_fd);
                 }
                 else if (!strcmp(comand,"pwd"))
                 {
                     printf("this is pwd part\n");
                     pwdFunction(new_fd);
                 }
                 else if (!strcmp(comand,"cd"))
                 {
                     recvCycle(new_fd,(char*)&datalen,sizeof(int));
                     recvCycle(new_fd,buf,datalen);
                     //对于用户输入的目录需要分情况讨论，分为/ 根目录
                    // if (!strcmp(buf,"/"))
                    // {
                    //     chdir(path);
                    // }
                    // if (!strcmp(getcwd(NULL,0),Home))
                    // {       //确保当前账户处于自己的home目录下面
                    //     chdir(path);
                    // }
                     printf("path=%s\n",path);
                     printf("home=%s\n",home);
                     cdFunc(buf,path,home);       //此处代码得做调整
                 }
                 else if (!strcmp(comand,"gets"))
                 {
                     recvCycle(new_fd,(char*)&datalen,sizeof(int));
                     recvCycle(new_fd,buf,datalen);
                     tranFile(new_fd,buf);
                 }
                 else if (!strcmp(comand,"puts"))
                 {
                     recvFile(new_fd);
                 }
                 else if (!strcmp(comand,"remove"))
                 {
                     recvCycle(new_fd,(char*)&datalen,sizeof(int));
                     recvCycle(new_fd,buf,datalen);
                     remove_dir(buf);
                     ERROR_CHECK(ret,-1,"remove");
                 }
                 else if (!strcmp(comand,"mkdir"))
                 {
                     recvCycle(new_fd,(char*)&datalen,sizeof(int));
                     recvCycle(new_fd,buf,datalen);
                     printf("buf=%s\n",buf);
                     mkdirFunction(buf);
                 }
                 else if (!strcmp(comand,"exit"))
                 {
                    // printf("Before goto switchAcc\n");
                     goto switchAcc;
                    // printf("After goto SwitchAcc\n");
                 }
             }
         }
      }
    }
    return 0;
}
