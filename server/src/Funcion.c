#include "../include/factory.h"
#include "../include/func.h"
#include "../include/work_que.h"

int Function(int new_fd)
{
     char comand[20]={0};             //用于接收命令
     char buf[1000]={0};
     int datalen;
     int  ret;
     char user[30]={0};
     char path[1024]={0};
     char home[1000]={0};
  switchAcc:
     memset(user,0,sizeof(user));
     logFunction(new_fd,user);
     memset(path,0,sizeof(path));
     strcpy(home,BASE_PATH);
     strcat(home,"/Home");
     printf("home=%s\n",home);
     sprintf(path,"%s/%s",home,user);       //将登录用户锁定在自己的 home 目录
    
     while (1)
     {
          memset(comand,0,sizeof(comand));            //读取客户端发送过来的命令
          memset(buf,0,sizeof(buf));                  //buf 清空，用于接收文件名或者路径名
          //printf("new_fd=%d\n",new_fd);
          recvCycle(new_fd,(char*)&datalen,sizeof(int));
          //printf("datalen=%d\n",datalen);
          recvCycle(new_fd,comand,datalen);
         // printf("comand=%s\n",comand);
          if (!strcmp(comand,"quit"))
          {
              printf("byebye\n");
              close(new_fd);
              break;
          }
          else if (!strcmp(comand,"ls"))
          {
              recvCycle(new_fd,(char*)&datalen,sizeof(int));
              recvCycle(new_fd,buf,datalen);
              lsFunction(buf,new_fd,user);
          }
          else if (!strcmp(comand,"pwd"))
          {
              printf("this is pwd part\n");
              pwdFunction(new_fd,user);
          }
          else if (!strcmp(comand,"cd"))
          {
              recvCycle(new_fd,(char*)&datalen,sizeof(int));
              recvCycle(new_fd,buf,datalen);
              printf("path=%s\n",path);
              printf("home=%s\n",home);
              cdFunc(user,buf,path,home);       //此处代码得做调整
          }
          else if (!strcmp(comand,"gets"))
          {
              pthread_mutex_t mutex;
              pthread_mutex_init(&mutex,NULL);
              pthread_mutex_lock(&mutex);
              char currentpath[1024]={0};
              path_query(user,currentpath);
              chdir(currentpath);
              recvCycle(new_fd,(char*)&datalen,sizeof(int));
              recvCycle(new_fd,buf,datalen);
              tranFile(new_fd,buf);
              pthread_mutex_unlock(&mutex);
          }
          else if (!strcmp(comand,"puts"))
          {
              pthread_mutex_t mutex;
              pthread_mutex_init(&mutex,NULL);
              pthread_mutex_lock(&mutex);
              char currentpath[1024]={0};
              path_query(user,currentpath);
              chdir(currentpath);
              recvFile(new_fd);
              pthread_mutex_unlock(&mutex);
          }
          else if (!strcmp(comand,"remove"))
          {
              pthread_mutex_t mutex;
              pthread_mutex_init(&mutex,NULL);
              pthread_mutex_lock(&mutex);
              char currentpath[1024]={0};
              path_query(user,currentpath);
              chdir(currentpath);
              recvCycle(new_fd,(char*)&datalen,sizeof(int));
              recvCycle(new_fd,buf,datalen);
              remove_dir(buf);
              pthread_mutex_unlock(&mutex);
              ERROR_CHECK(ret,-1,"remove");
          }
         else if (!strcmp(comand,"mkdir"))
         {
             recvCycle(new_fd,(char*)&datalen,sizeof(int));
             recvCycle(new_fd,buf,datalen);
             printf("buf=%s\n",buf);
             mkdirFunction(buf,user);
         }
         else if (!strcmp(comand,"exit"))
         {
             goto switchAcc;
         }
     }
}
