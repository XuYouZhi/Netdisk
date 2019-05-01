#include "../include/factory.h"
#include "../include/func.h"
#include "../include/work_que.h"
#include "../include/md5.h"

int Function(int new_fd)
{
     char comand[20]={0};             //用于接收命令
     char buf[1000]={0};
     int datalen;
     int  ret;
     char user[30]={0};
     char path[1024]={0};
     char home[1000]={0};
     char oldPath[1000]={0};    //此变量用于file_query 中的传出参数
     char md5Value[100]={0};
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
              //接收客户端发送过来的md5值
              recvCycle(new_fd,(char*)&datalen,sizeof(int));
              memset(buf,0,sizeof(buf));
              recvCycle(new_fd,buf,datalen);
              printf("md5Value=%s\n",buf);
              strcpy(md5Value,buf);
              ret=file_query(buf,oldPath);      // 如果已经上传过相同md5码文件,
              //ret 也应该作为返回值返回给客户端
              //  t.datalen=sizeof(ret);
              //  memcpy(t.buf,&ret,sizeof(int));
              //  sendCycle(new_fd,(char*)&t,4+t.datalen);
              printf("ret=%d\n",ret);
              sendCycle(new_fd,(char*)&ret,sizeof(int));
            
              char currentpath[1024]={0};        //切换到当前用户所在的目录
              path_query(user,currentpath);
              chdir(currentpath);
              if (-1==ret)                      //返回0, 否则返回1
              {//返回值为-1，说明没有用户上传过 此 md5码所对应的文件
                 char fileName[100]={0};                //此为正常上传文件部分
                 recvFile(new_fd,fileName);             //fileName通过指针的形式作为返回值
                 printf("fileName=%s\n",fileName);
                 //将此 md5记录到数据库中
                 char filePath[1024]={0};
                 sprintf(filePath,"%s/%s",currentpath,fileName);
                 printf("filePath 1=%s\n",filePath);
                 file_insert(fileName,filePath,buf);    //将刚刚上传的文件md5值插入数据库中
                 fileNum_update(buf,'+');           //将 fileNum 值加1
              }
              else
              {
                  // 此部分是制作待上传文件硬链接部分的代码逻辑
                  //秒传功能
                // 首先接收客户端发送过来的文件名
                 memset(buf,0,sizeof(buf));
                 recvCycle(new_fd,(char*)&datalen,sizeof(int));
                 recvCycle(new_fd,buf,datalen);
                 printf("fileName=%s\n",buf);
                 char filePath[1024]={0};
                 sprintf(filePath,"%s/%s",currentpath,buf);
                 printf("filePath 2=%s\n",filePath);
                 link(oldPath,filePath);        //调用库函数制作硬链接
                 int success=1;         //告知客户端硬链接制作完成
                 sendCycle(new_fd,(char*)&success,sizeof(int));
                 fileNum_update(md5Value,'+');      //将数据库中指向此文件链接数加1
              }
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
              char md5Str[33]={0};
              Compute_file_md5(buf,md5Str);
              printf("md5Str=%s\n",md5Str);
              fileNum_update(md5Str,'-');
              file_delete();
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
