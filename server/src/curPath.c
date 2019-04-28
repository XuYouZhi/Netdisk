#include "../include/func.h"


//对curPath进行插入操作
int path_insert(char *username,char* currentPath)        //curpath 此处是小写表示当前路径
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[1000]={0};       //用于存储进行insert操作的sql语句
    sprintf(query,"insert into curPath(username,path) values('%s','%s')",username,currentPath); 
    puts(query);        //将 sql 语句打印输出看一下
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		printf("insert success\n");
	}
	mysql_close(conn);
	return 0;
}


//对curPath 进行查询操作
int path_query(char *username,char *currentPath)        //函数中的 currentpath 作为返回值返回给调用函数部分
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select path from curPath where username='";
	sprintf(query,"%s%s'",query,username);
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
						printf("%8s ",row[t]);      //此处输出用户当前所处路径
                        strcpy(currentPath,row[t]);
				}
				printf("\n");
			}
		}else{
			printf("Don't find data\n");
            return -1;          //返回-1表示在 curPath 表中没有找到对应用户的当前目录
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;               //返回0表示在 curPath 表中找到对应用户当前所处的路径
}


int path_query1(char *username)        //用于查询当前用户在 curPath中是否有存储路径
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select path from curPath where username='";
	sprintf(query,"%s%s'",query,username);
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
						printf("%8s ",row[t]);      //此处输出用户当前所处路径
				}
				printf("\n");
			}
		}else{
			printf("Don't find data\n");
            return -1;          //返回-1表示在 curPath 表中没有找到对应用户的当前目录
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;               //返回0表示在 curPath 表中找到对应用户当前所处的路径
}

//对 curPath 进行修改操作
int path_update(char *username,char* currentPath)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[1200]="update curPath set path='"; 
	sprintf(query,"%s%s%s%s%s%s",query,currentPath,"'", " where username='",username,"'");
	puts(query);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}







//int main(int argc,char *argv[])
//{
//    ARGS_CHECK(argc,3);
//    //path_insert(argv[1],argv[2]);
//    //path_query(argv[1]);
//    path_update(argv[1],argv[2]);
//    return 0;
//}
