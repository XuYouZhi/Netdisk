#include "../include/func.h"
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
            ret= tranFile(socketFd,comand2);//comand2为文件名
            if (-1==ret)
            {
                printf("puts error\n");
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

            //下载文件时涉及到断点续传问题，得先判断本地文件是否存在，如果存在则得将本地文件的大小发送给服务器
            chdir("UpAndDownLoad");         //切换至本地下载目录
            ret=access(comand2,F_OK);
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
