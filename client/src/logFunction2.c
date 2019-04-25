#include "../include/func.h"
//void getsalt(char *salt,char* passwd)
//{
//	int i,j;
//	for(i=0,j=0;passwd[i]&&j!=3;i++)
//	{
//		if(passwd[i]=='$')
//		{
//			++j;
//		}
//	}
//	strncpy(salt,passwd,i-1);
//}
//
//
//int logFunction(char  *argv)
//{
//	struct spwd *sp;
//	char *passwd;
//	char salt[512]={0};
//   // if(argc!=2)
//   // {
//   // 	printf("error args\n");
//   // 	return -1;
//   // }
//	passwd=getpass("请输入密码:");
//	sp=getspnam(argv);
//	if(NULL==sp)
//	{
//		perror("getspnam");
//		return -1;
//	}
//	//printf("%-100s\n",sp->sp_pwdp);
//	getsalt(salt,sp->sp_pwdp);
//	//puts(salt);
//	if(!strcmp(sp->sp_pwdp,crypt(passwd,salt)))	
//	{
//		printf("登录成功\n");
//        return 1;
//	}else{
//		printf("登录失败\n");
//        return -1;
//	}	
//}



int GenerateStr(char *salt)
{
    char str[8+1]={0};
    int i,flag;
    srand(time(NULL));          //通过时间函数设置种子，使得每次运行的结果随机
    for (i=0;i<8;++i)
    {
        flag=rand()%3;
        switch(flag)
        {
        case 0:
            str[i]=rand()%26+'a';
            break;
        case 1:
            str[i]=rand()%26+'A';
            break;
        case 2:
            str[i]=rand()%10+'0';
            break;
        }
    }
    strcpy(salt,"$6$");         //采用 sha512加密算法
    strcat(salt,str);
    return 0;
}

char* encryptPasswd(char *salt,char *password)
{
    password=crypt(password,salt);
    return password;
}
char* logFunction1(int socketFd)        //本函数的参数为 sfd
{
    int ret;
    char flag=0;
    printf("直接登录请输入y  注册账号请输入 r\n");
    scanf("%c",&flag);
    if ('r'==flag||'R'==flag||'y'==flag||'Y'==flag)
    {
        sendCycle(socketFd,&flag,sizeof(char));
    }
    else
    {
        printf("输入错误!\n");
        exit(-1);
    }
    Train t;
    static char ch[3][30];
    memset(ch,0,sizeof(ch));
    int dataLen;
    char buf[100]={0};
    static int count=0;
    if ('y'==flag||'Y'==flag)       //登录部分逻辑
    {
login2:
        system("clear");
        memset(ch[0],0,sizeof(ch[0]));
        printf("请输入用户名: ");
        scanf("%s",ch[0]);
        //将用户名发送给服务器，用于请求salt值
        strcpy(t.buf,ch[0]);
        t.datalen=strlen(ch[0]);
        sendCycle(socketFd,(char*)&t,4+t.datalen);
        //接收服务器发送来的salt值
        memset(buf,0,sizeof(buf));
        recvCycle(socketFd,(char*)&dataLen,sizeof(int));
        recvCycle(socketFd,buf,dataLen);
       // printf("salt=%s\n",buf);
        memset(ch[1],0,sizeof(ch[1]));
        strcpy(ch[1],getpass("请输入密码: "));
        //根据用户输入的密码和从服务器端发送过来的salt值加密得到密文passwword 
        memset(&t,0,sizeof(t));
        strcpy(t.buf, encryptPasswd(buf,ch[1]));
        t.datalen=strlen(t.buf);
        sendCycle(socketFd,(char*)&t,4+t.datalen);
        // 接收服务器匹配密码后的结果[y/n]
        memset(buf,0,sizeof(buf));
        recvCycle(socketFd,buf,1);
        if (!strcmp(buf,"y"))
        {
            printf("登录成功!欢迎使用!\n");
        }
        else
        {
            printf("密码错误或用户名错误!\n");
            sleep(1);
            ++count;
           //printf("count=%d\n",count);
            if (count<3)
            {//允许输错三次密码
                goto login2;
            }
            else if (count>=3)
            {
                exit(-1);
            }
        }
    }
    else if ('r'==flag||'R'==flag)      //注册部分逻辑
    {
        char salt[20];
        char password[1000]={0};
        memset(ch[0],0,sizeof(ch[0]));   //ch[0]用于存储输入的用户名
        memset(salt,0,sizeof(salt));
        printf("-----------------------注册新账号------------------------\n");        
        printf("请输入一个用户名(提示,此用户名将作为你的登录账号)：");
        scanf("%s",ch[0]);
login1:
        memset(ch[1],0,sizeof(ch[1]));
        memset(ch[2],0,sizeof(ch[2]));
        strcpy(ch[1],getpass("请输入密码:"));
        strcpy(ch[2],getpass("请确定密码:"));
        if (strcmp(ch[1],ch[2])!=0)
        {
            printf("两次输入的密码不一致!,请重新输入\n");
            goto login1;
        }
        strcpy(password,ch[1]);
       //printf("before encrypt=%s\n",password);
        printf("注册成功!\n");
      //发送用户名给服务器
        t.datalen=strlen(ch[0]);
        strcpy(t.buf,ch[0]);
        sendCycle(socketFd,(char*)&t,4+t.datalen);
        //发送 salt值给服务器
        GenerateStr(salt);
      //printf("salt=%s\n",salt);
        strcpy(t.buf,salt);
        t.datalen=strlen(salt);
        sendCycle(socketFd,(char*)&t,4+t.datalen);
        //发送密码加密后的密文给服务器
        strcpy(password,encryptPasswd(salt,password));
       //printf("After encrypt,password=%s\n",password);
        strcpy(t.buf,password);
        t.datalen=strlen(password);
        sendCycle(socketFd,(char*)&t,4+t.datalen);
        goto login2;            //注册完毕，转为登录
    }
    return ch[0];           //返回登录的用户名
}

