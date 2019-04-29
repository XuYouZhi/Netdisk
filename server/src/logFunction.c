#include "../include/func.h"

int logFunction(int new_fd,char *user)
{
    Train t;
    char path[1024]={0};
    char flag=0;
    int ret;
    recvCycle(new_fd,&flag,sizeof(char));
    //printf("flag=%c\n",flag);
    char salt[20]={0};
    int dataLen;
    char buf[1000]={0};
    char password[1000]={0};
    char userName[300];
    char userName2[300]={0};
    int count=0;
    if ('y'==flag||'Y'==flag)       //登录部分代码逻辑
    {
login:
       memset(salt,0,sizeof(salt));
       memset(userName,0,sizeof(userName));
       memset(buf,0,sizeof(buf));
       //接收客户端方向传送过来的用户名
       recvCycle(new_fd,(char*)&dataLen,sizeof(int));
       recvCycle(new_fd,buf,dataLen);
       //printf("userName=%s\n",buf); 
       strcpy(userName,buf);
       strcpy(user,buf);
       strcpy(userName2,userName);
       my_query("salt",buf);    //查询数据库中 username 账户对应的 salt 值
      // printf("salt=%s\n",buf);
       //将salt发送给客户端
        t.datalen=strlen(buf);
        strcpy(t.buf,buf);
        ret=sendCycle(new_fd,(char*)&t,4+t.datalen);
        if (-1==ret)
        {
            return -1;
        }
        //接收客户端发送过来的加密后的password
        recvCycle(new_fd,(char*)&dataLen,sizeof(int));
        recvCycle(new_fd,buf,dataLen);
       // printf("password=%s\n",buf);
        my_query("password",userName);
       // printf("query passwd=%s\n",userName);
        if (!strcmp(buf,userName))      //将客户端发送过加密过的密文与服务器端数据库中存储的密文进行比对
        {                               //如果相同，则返回客户端' y'字符
           // printf("current path=%s\n",getcwd(NULL,0));
            ret=sendCycle(new_fd,"y",1);
            if (-1==ret)
            {
                return -1;
            }
           // printf("userName2=%s\n",userName2);
           //printf("current path=%s\n",getcwd(NULL,0));
            sprintf(path,"%s/Home/%s",BASE_PATH,userName2);
           // printf("logFunction part,curPath=%s\n",path);
            ret=path_query1(userName2);
            //printf("ret=%d\n",ret);
            if (-1==ret)
            {
               // printf("insert current path into curPath table\n");
                path_insert(userName2,path);    //将当前登录用户所处的目录存入数据库
            }
            else
            {
                //printf("update current path into curPath table\n");
                path_update(userName2,path);            //将当前用户所处的路径同步到curPath 表中
            }
            chdir(path);
            //printf("logFunction part,path=%s\n",path);
           // printf("current path=%s\n",getcwd(NULL,0));
        }else
        {                           //比对结果不相同，返回客户端'n'字符
            ret=sendCycle(new_fd,"n",1);
            if (-1==ret)
            {
                return -1;
            }
            ++count;                    //允许三次输入密码错误
            if (count<3)
            {
                goto login; 
            }
            if (count>=3)
            {
                close(new_fd);  //断开客户端与服务器的连接
            }

        }
       // printf("count=%d\n",count);
    } 
    else if ('r'==flag||'R'==flag)      //注册部分代码逻辑
    {
        memset(salt,0,sizeof(salt));
        memset(password,0,sizeof(password));
        memset(userName,0,sizeof(userName));
        memset(buf,0,sizeof(buf));
        //接收用户名
        recvCycle(new_fd,(char*)&dataLen,sizeof(int));
        recvCycle(new_fd,userName,dataLen);
       // printf("userName=%s\n",userName);
        //接收盐值
        recvCycle(new_fd,(char*)&dataLen,sizeof(int));
        recvCycle(new_fd,salt,dataLen);
       // printf("salt=%s\n",salt);
        //接收密码加密后的密文
        recvCycle(new_fd,(char*)&dataLen,sizeof(int));
        recvCycle(new_fd,password,dataLen);
        //printf("password=%s\n",password);
        //将接收到的数据存储存入数据库中 
        my_insert(userName,salt,password);
        //设置当前用户的home 目录
       // printf("current path=%s\n",getcwd(NULL,0));//下面采用这种手动固定给定路径，很不好，使得程序不具备移植性
        sprintf(path,"%s/Home",BASE_PATH);
        chdir(path);             //切换到home目录下面
        mkdir(userName,0777);        //创建当前用户的小家
        chdir(userName);        //切换至当前用户下的home目录下面
        goto login;
    }
    return 0;
}

int my_insert(char *userName,char *salt,char *password)
{
    MYSQL *conn;
    char* server=(char*)"localhost";
    char* user=(char*)"root";
    char* passwd=(char*)"xyz";
    char* database=(char*)"test";
    char query[1000]={0};               //用于存储数据库的 sql 语句
    sprintf(query,"insert into User(username,salt,password) values('%s','%s','%s')",userName,salt,password);
    puts(query);        //将 sql 语句打印输出看一下
    int t;
    conn=mysql_init(NULL);
    if (!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }
    else
    {
        printf("Connected...\n");
    }
    t=mysql_query(conn,query);
    if (t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }
    else
    {
        printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}
int my_query(const char *ch,char *userName)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]={0};
    //"select * from User where username='";
	//sprintf(query,"%s%s%s",query, argv[1],"'");
    //strcpy(query,"select * from Person");
    if (!strcmp(ch,"salt"))
    {//查salt值
        sprintf(query,"select salt from User where username='%s'",userName);
    }
    else if (!strcmp(ch,"password"))
    {//查密文password
        sprintf(query,"select password from User where username='%s'",userName);
    }
	puts(query);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
	//	printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{	
				//printf("num=%d\n",mysql_num_fields(res));//列数
				for(t=0;t<mysql_num_fields(res);t++)
				{
					//	printf("%8s ",row[t]);
                    strcpy(userName,row[t]);
				}
				printf("\n");
			}
		}else{
			printf("Don't find data\n");
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
