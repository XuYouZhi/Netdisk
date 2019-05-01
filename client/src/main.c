#include "../include/func.h"
#include "../include/md5.h"
#include "../include/progress.h"            //秒传部分需要打印100%进度条,
                                            //故主函数中也包含此头文件
int main(int argc,char *argv[])
{
    int ret;
    ARGS_CHECK(argc,3);         //命令行中需要输入2个参数，依次是IP，Port
    int socketFd=socket(AF_INET,SOCK_STREAM,0);                   //初始化网络套接字
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in serAddr;
    memset(&serAddr,0,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(atoi(argv[2]));                      //将端口转换成网络字节序
    serAddr.sin_addr.s_addr=inet_addr(argv[1]);                  //将IP地址转换成网络字节序
    ret=connect(socketFd,(struct sockaddr*)&serAddr,sizeof(struct sockaddr));
    ERROR_CHECK(ret,-1,"connect");
    int dataLen;
    char userName[100]={0};
    char buf[1000];
    Train t;            //定义发送数据的小火车
login:
    memset(userName,0,sizeof(userName));
    strcpy(userName,logFunction1(socketFd));         // 选择登录还是注册代码逻辑
    getchar();          //读走输入缓冲区中多余的换行符
begin:
    system("clear");
    printf("please enter the command\n");
    printf("\t1.cd  进入对应目录\n"
           "\t2.ls 列出相应目录文件\n"
           "\t3.puts 将本地文件上传至服务器\n"
           "\t4.gets 文件名 下载服务器文件到本地\n"
           "\t5.remove 删除服务器上文件\n"
           "\t6.pwd 显示目前所在路径\n"
           "\t7.mkdir 创建新目录\n"
           "\t8.exit 切换当前账户\n"
           "\t9.quit 退出登录\n");
    char comand[1024]={0};            //用于存储输入的命令
    char comandTmp[1024]={0};
    char *comandUnion[]={"cd","ls","puts","gets","remove","pwd","mkdir","exit","quit"};
    memset(&t,0,sizeof(t));
    while (fgets(comand,sizeof(comand),stdin)!=NULL)
    {
        if (!strcmp(comand,"\n"))
        {
            continue;   //如果输入的是换行符，则重新输入，提高程序的健壮性
                        //避免程序不小心崩溃
        }
        strcpy(comandTmp,comand);
        comandTmp[strlen(comandTmp)-1]='\0';
        
        //此处添加日志记录函数，记录某用户的每次操作
        logRecord(userName,comandTmp);
        char *token=strtok(comand," \n");
        char comand1[20]={0};
        strcpy(comand1,token);
        int i;
        int flag=0;
        for (i=0;i<9;++i)           //判断输入的指令是否合法
        {
            if (!strcmp(comand1,comandUnion[i]))
            {
                flag=1;
                break;
            }
        }
        if (!flag)
        {
            logRecord(userName,"comand error,Please enter the comand again");
            printf("comand error,Please enter the comand again\n");
            printf("please enter any key to continue\n");
            getchar();
            goto begin;
        }
        t.datalen=strlen(comand1);
        strcpy(t.buf,comand1);
        ret=sendCycle(socketFd,(char*)&t,4+t.datalen);      //此处的4表示 sizeof(datalen);
        if (!strcmp(comand1,"exit"))
        {
           // getchar();              //读走缓冲区中的多余的'\n'字符
            goto login;
        }

        char comand2[1024]={0};
        if ((!strcmp(comand1,"ls")&&strlen(comandTmp)==2)||(!strcmp(comand1,"pwd")))
        {
            strcpy(comand2,".");
        }
        else if (!strcmp(comandTmp,"quit"))
        {
            //不执行任何操作，写这条语句是避免程序进入 else 分支
        }
        else
        {
            if (NULL!=token)
            {
                token=strtok(NULL,"\n");
                strcpy(comand2,token);
            }
        }
        switch(i)
        {
        case 0:         //this is cd function
            strcpy(t.buf,comand2);
            t.datalen=strlen(t.buf);
           // printf("comand2=%s\n",comand2);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                return -1;
            }
            break;
        case 1:         //this is ls function
            t.datalen=strlen(comand2);
            strcpy(t.buf,comand2);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                return -1;
            }
            while (1)
            {
                memset(buf,0,sizeof(buf));
                recvCycle(socketFd,(char*)&dataLen,sizeof(int));
                if (0==dataLen)
                {
                    break;
                }
                recvCycle(socketFd,buf,dataLen);
                printf("%s\n",buf);
            }
            break;
        case 2:             //this is puts function
            //在正式上传文件之前，先将待上传的文件的 md5发送给服务器
            //如果服务器端应该有客户端上床过相同 md5的文件，
            //可以通过做硬链的方式，实现秒传。
            //秒传部分注意对进度条打印部分的处理
            ;       //此处加一个分号的目的是增加一个空语句
            //不加分号，会报错，a label can only be part of a statement and a declaration is not a statement
         //   printf("this is puts parts\n");
            char md5Str[MD5_STR_LEN+1];    //用于存储32位的文件 md5值
          //  printf("comand2=%s\n",comand2);
            char buffer[1000]={0};
            // 切换到待上传文件所在的 目录
            sprintf(buffer,"%s/%s",PATH,"UpAndDownLoad");
            chdir(buffer);
            ret=Compute_file_md5(comand2,md5Str);
            if (0==ret)
            {
               // printf("[file-%s] md5 value:\n",comand2);
               // printf("%s\n",md5Str);
            }
            
            //采用小火车方式，将待上传文件的 md5码值传给服务器
            t.datalen=strlen(md5Str);
            strcpy(t.buf,md5Str);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                printf("failed to send md5Value.\n");
            }
           
            recvCycle(socketFd,(char*)&flag,sizeof(int));
//            printf("puts part,flag=%d\n",flag);
            if (-1==flag)       //返回值为-1,说明server 端无对应 md5所对应的文件
            {
                //如果服务器端没有存相同md5表的文件，这才真正上传文件
                 ret= tranFile(socketFd,comand2);//comand2为文件名
                 if (-1==ret)
                 {
                     printf("puts error\n");
                 }
            }
            else
            {
                //flag 为0，说明 server 端已经有上传过此 md5码对应的文件
                //此时只需要将待上传文件的文件名发送给服务端，便于服务端制作硬链接
                 t.datalen=strlen(comand2);
                 strcpy(t.buf,comand2);
                 ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
                 if (-1==ret)
                 {
                     return -1;
                 }
                 recvCycle(socketFd,(char*)&dataLen,sizeof(int));
                 printf("dataLen=%d\n",dataLen);
                 if (1==dataLen)
                 {
                     //秒传部分，客户端打印100%进度条
                     progress_t bar;
                     progress_init(&bar, "",50, PROGRESS_CHR_STYLE);
                     progress_show(&bar,50/50.0f);
                     printf("\n+-Done\n");
                     progress_destroy(&bar);
                 }
            }
            break;
        case 3:             //this is gets function
            memset(t.buf,0,sizeof(t.buf));          //发送文件名给服务器
            t.datalen=strlen(comand2);
            strcpy(t.buf,comand2);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                return -1;
            }
            printf("current path=%s\n",getcwd(NULL,0));

            //下载文件时涉及到断点续传问题，得先判断本地文件是否存在，如果存在则得将本地文件的大小发送给服务器
            chdir("UpAndDownLoad");         //切换至本地下载目录
            ret=access(comand2,F_OK);
            printf("ret=%d\n",ret);
            dataLen=ret;
            // 通过ret 返回值向服务器报告本地是否存在待下载文件
            ret=sendCycle(socketFd,(char*)&dataLen,sizeof(int));
            printf("dataLen=%d\n",dataLen);
            if (-1==ret)
            {
                return -1;
            }
            if (0==dataLen)
            {
                //如果本地存在文件，则应该将本地文件的大小传递给服务器
                 int fd=open(comand2,O_RDONLY);
                 ERROR_CHECK(fd,-1,"fopen");
                 struct stat file_sta;
                 fstat(fd,&file_sta);
                 printf("%ld\n",file_sta.st_size);
                printf("%s exits\n",comand2);
                ret=sendCycle(socketFd,(char*)&file_sta.st_size,sizeof(file_sta.st_size));
                if (-1==ret)
                {
                    return -1;
                }
            }
            printf("dataLen=%d\n",dataLen);
            recvFile(socketFd,dataLen);
            break;
        case 4:             //this is remove function
            t.datalen=strlen(comand2);
            strcpy(t.buf,comand2);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                return -1;
            }
            recvCycle(socketFd,(char*)&flag,sizeof(int));
            printf("remove part,flag=%d\n",flag);
            if (-1==flag)
            {
                printf("remove %s error\n",comand2);
            }
            break;
        case 5:                 //this is pwd function
            while (1)
            {
            //    printf("this is pwd part\n");
                memset(buf,0,sizeof(buf));
                recvCycle(socketFd,(char*)&dataLen,sizeof(int));
                if (0==dataLen)
                {
                    break;
                 }
                recvCycle(socketFd,buf,dataLen);
                char temp[50]={0};
                strcpy(temp,"/");
                strcat(temp,userName);
                char *curPath=strstr(buf,temp);
                if ((strlen(curPath)-1)==strlen(userName))//打印当前用户的根目录
                {
                    printf("/");
                }
                printf("%s\n",curPath+strlen(userName)+1);
            }
            break;
        case 6:            //this is mkdir function
            memset(t.buf,0,sizeof(t.buf));
            t.datalen=strlen(comand2);
            strcpy(t.buf,comand2);
            ret=sendCycle(socketFd,(char*)&t,4+t.datalen);
            if (-1==ret)
            {
                return -1;
            }
            break; 
        case 8:
            goto end;       //程序结束
            break;
        default:
            break;
        }
        printf("please enter RETURN key to continue\n");
       char ch= getchar();
       while(ch!='\n')
       {
           ch=getchar();
       }
        goto begin; 
    }
end:
    return 0;
}
