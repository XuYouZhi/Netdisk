#include "../include/func.h"
int file_insert(char* fileName,char* filePath,char* md5Val)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[1200]="insert into fileMd5(fileName,filePath,md5) ";
	int t,r;
    sprintf(query,"%s values('%s','%s','%s')",query,fileName,filePath,md5Val);
    puts(query);        //将插入语句打印输出看一下
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

int fileNum_update(char *md5Val,char select)        //此处的 select 字符可以输入+或者-，用于对 fileNum 进行+1或-1操作
{
    MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
    char query[1200]="update fileMd5 set fileNum=fileNum";
	int t,r;
    sprintf(query,"%s%c1 where md5='%s'",query,select,md5Val);
    puts(query);        //将插入语句打印输出看一下
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



int file_delete()
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[200]="delete from fileMd5 where fileNum=0";
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
		printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
	}
	mysql_close(conn);
	return 0;
}


int file_query(char* md5Val,char* oldPath)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select filePath from fileMd5 where md5='";
	sprintf(query,"%s%s'",query,md5Val);
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
        return -1;
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
						printf("%8s ",row[t]);
                        strcpy(oldPath,row[t]);
				}
				printf("\n");
			}
            if (NULL==row&&!t)
            {
                return -1;
            }
		}else{
			printf("Don't find data\n");
            return -1;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

//int file_update(int argc,char* argv[])
//{
//	if(argc!=2)
//	{
//		printf("error args\n");
//		return -1;
//	}
//	MYSQL *conn;
//	MYSQL_RES *res;
//	MYSQL_ROW row;
//	char* server="localhost";
//	char* user="root";
//	char* password="123";
//	char* database="test";
//	char query[200]="update Person set LastName='";
//	sprintf(query,"%s%s%s",query,argv[1],"' where personID=5");
//	puts(query);
//	int t,r;
//	conn=mysql_init(NULL);
//	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
//	{
//		printf("Error connecting to database:%s\n",mysql_error(conn));
//	}else{
//		printf("Connected...\n");
//	}
//	t=mysql_query(conn,query);
//	if(t)
//	{
//		printf("Error making query:%s\n",mysql_error(conn));
//	}else{
//		printf("update success\n");
//	}
//	mysql_close(conn);
//	return 0;
//}

//int main()
//{
//   // char *fileName="file";
//   // char *filePath="/home/xuyouzhi/Netv1.2/server/Home/huang";
//   // char *md5Val="6f5902ac237024bdd0c176cb93063dc4";
//   // file_insert(fileName,filePath,md5Val);
//    //fileNum_Update("6f5902ac237024bdd0c176cb93063dc4",'-');
//   // char oldPath[1000]={0};
//   // int ret=file_query("aaf7c9fb85ac29d8956cf132201aec70",oldPath);
//   // printf("oldPath=%s\n",oldPath);
//   // printf("ret=%d\n",ret);
//    file_delete();
//    return 0;
//}
