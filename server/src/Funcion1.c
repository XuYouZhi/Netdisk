#include "../include/factory.h"
#include "../include/func.h"
#include "../include/work_que.h"
#include "../include/md5.h"
#include "../include/generateToken.h"

int Function1(int new_fd)
{
    printf("Function1 part\n");
     char comand[20]={0};             //用于接收命令
     char buf[1000]={0};
     int datalen;
     int  ret;
     Train t;
     char user[30]={0};
     char path[1024]={0};
     char home[1000]={0};
     char oldPath[1000]={0};    //此变量用于file_query 中的传出参数
     char md5Value[100]={0};
     int select;            //用于具体确认是哪个选项
     char tokenValue[65]={0};
     recvCycle(new_fd,(char*)&select,sizeof(int));
     printf("select=%d\n",select);
    if (0==select)
    {
    switchAcc:
         memset(user,0,sizeof(user));
         logFunction(new_fd,user);
         printf("user=%s\n",user);
         //在生成token之前，先查询数据库中是否有此用户名对应的token值
         ret=token_query1(user);
         if (-1==ret)
         {
             memset(tokenValue,0,sizeof(tokenValue));
             GenerateToken(user,tokenValue);
             printf("tokenValue=%s\n",tokenValue);
             // 服务器端需要将 token 值存入数据库中，同时将 token 值返回给客户端
             token_insert(user,tokenValue);
         }
         //查表获得当前用户名对应的token值
        token_query2(user,tokenValue);
        printf("query2,tokenValue=%s\n",tokenValue);
        t.datalen=strlen(tokenValue);
        strcpy(t.buf,tokenValue);
        ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
        // memset(path,0,sizeof(path));
        // strcpy(home,BASE_PATH);
        // strcat(home,"/Home");
        // printf("home=%s\n",home);
        // sprintf(path,"%s/%s",home,user);       //将登录用户锁定在自己的 home 目录
    }
    else
    {
        
         memset(comand,0,sizeof(comand));            //读取客户端发送过来的命令
         memset(buf,0,sizeof(buf));                  //buf 清空，用于接收文件名或者路径名
         recvCycle(new_fd,(char*)&datalen,sizeof(int));
         recvCycle(new_fd,comand,datalen);
         printf("comand=%s\n",comand);
        //接收客户端的token值,用于登陆验证
         if (!strcmp(comand,"exit"))
         {
            goto switchAcc;
         }
         else
         {  memset(buf,0,sizeof(buf));
            recvCycle(new_fd,(char*)&datalen,sizeof(int));
            recvCycle(new_fd,buf,datalen);
            printf("tokenValue=%s\n",buf);
            token_query(user,buf);         //根据 token 值查表得到当前登陆用户的用户名
            printf("userName=%s\n",user);
        
            memset(path,0,sizeof(path));
            memset(home,0,sizeof(home));
            strcpy(home,BASE_PATH);
            strcat(home,"/Home");
        // printf("home=%s\n",home);
            sprintf(path,"%s/%s",home,user);       //将登录用户锁定在自己的 home 目录
        
            printf("begin part\n");
            printf("home=%s\n",home);
            printf("path=%s\n",path);

            memset(buf,0,sizeof(buf));      //用于将buf置空


         }
         if (!strcmp(comand,"quit"))
         {
             printf("byebye\n");
             return -1;
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
             Compute_file_md5(buf,md5Str);     //计算待删除文件的md5值
             //printf("md5Str=%s\n",md5Str);
             fileNum_update(md5Str,'-');       //更新数据库中的记录
             file_delete();
             ret=remove_dir(buf);          //删除服务器端用户对应的文件，如果是目录，则需要递归删除
             ERROR_CHECK(ret,-1,"remove");
             //通知客户端删除成功, 因为服务器端计算文件的md5时间较长，
             //客户端如果时序关系跟服务器端对不上，会对客户端的后续操作造成影响
             sendCycle(new_fd,(char*)&ret,sizeof(int));
             pthread_mutex_unlock(&mutex);
         }
        else if (!strcmp(comand,"mkdir"))
        {
            recvCycle(new_fd,(char*)&datalen,sizeof(int));
            recvCycle(new_fd,buf,datalen);
            //printf("buf=%s\n",buf);
            mkdirFunction(buf,user);
        }
        
    }
    return 0;
}
