#include "../include/generateToken.h"
#include "../include/func.h"

char* StrSHA256(const char* str, int length, char* sha256)
{
    /*
    计算字符串SHA-256
    参数说明：
    str         字符串指针
    length      字符串长度
    sha256         用于保存SHA-256的字符串指针
    返回值为参数sha256
    */
    char *pp, *ppend;
    int l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
    H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
    int K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };
    l = length + ((length % 64 >= 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (char*)malloc((unsigned int)l))) return 0;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((int*)(pp + l - 4)) = length << 3;
    *((int*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64){
        for (i = 0; i < 16; W[i] = ((int*)pp)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    free(pp - l);
    sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
    return sha256;
}

int GenerateToken(char *user,char *tokenValue)
{
    char text[100];
    strcpy(text,user);
    char sha256[65];
    StrSHA256(text,strlen(text),sha256);  // sizeof()计算的结果包含了末尾的'\0'应减1
    puts(sha256);
    
    time_t timep; 
    time(&timep); 
    char s[30]={0};
    memset(s,0,sizeof(s));
    strcpy(s,ctime(&timep));
    s[strlen(s)-1]='\0';
    printf("%s lenth=%ld\n",s,strlen(s)); 
    strcat(text,s);
    puts(text);
    printf("bytes of text is=%ld\n",sizeof(text));
    StrSHA256(text,strlen(text),sha256);  // sizeof()计算的结果包含了末尾的'\0'应减1
    puts(sha256);
    strcpy(tokenValue,sha256);
    return 0;
}

//对TokenValue进行插入操作
int token_insert(char *username,char* tokenValue)        
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[1000]={0};       //用于存储进行insert操作的sql语句
    sprintf(query,"insert into TokenValue(userName,tokenval) values('%s','%s')",username,tokenValue);
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


//对TokenValue进行查询操作
int token_query(char *username,char *tokenValue)        //函数中的username作为返回值返回给调用函数部分
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select userName from TokenValue where tokenval='";
	sprintf(query,"%s%s'",query,tokenValue);
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
						printf("%8s ",row[t]);      //此处输出当前token值对应的用户名
                        strcpy(username,row[t]);
				}
				printf("\n");
			}
		}else{
			printf("Don't find data\n");
            return -1;          //返回-1表示在TokenValue 表中没有找到token值对应的用户
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;               //返回0表示在 TokenValue 表中找到对应用户当前所处的路径
}


int token_query1(char *username)        //用于查询当前用户在TokenValue表中是否存在对应的token值
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select userName from TokenValue where userName='";
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
        return -1;
	}else{
	//	printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{	
			//	printf("num=%d\n",mysql_num_fields(res));//列数
				for(t=0;t<mysql_num_fields(res);t++)
				{
              //      printf("this is path_query1 part=%s\n",username);
			//			printf("%8s ",row[t]);      //此处输出 用户的用户名
				       
                }
				printf("\n");
			}
            if (NULL==row&&!t)
            {
                return -1;
            }
		}else{
			printf("Don't find data\n");
            return -1;          //返回-1表示在 TokenValue 表中没有找到对应用户的token值
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;               //返回0表示在 TokenValue 表中找到对应用户的token值
}

//对TokenValue进行查询操作
int token_query2(char *username,char *tokenValue)        //函数中的tokenValue作为返回值返回给调用函数部分
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";//要访问的数据库名称
	char query[300]="select tokenval from TokenValue where userName='";
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
						printf("%8s ",row[t]);      //此处输出当前用户对应的token值
                        strcpy(tokenValue,row[t]);
				}
				printf("\n");
			}
		}else{
			printf("Don't find data\n");
            return -1;          //返回-1表示在TokenValue 表中没有找到token值对应的用户
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;               //返回0表示在 TokenValue 表中找到对应用户当前所处的路径
}


//对TokenValue进行修改操作,你打算定期对数据库中的token值进行更新
//不过此部分功能尚处于构思阶段, 抓紧啊！！老哥
int token_update(char *username,char* tokenValue)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="xyz";
	char* database="test";
	char query[1200]="update TokenValue set tokenval='";
	sprintf(query,"%s%s%s%s%s%s",query,tokenValue,"'", " where userName='",username,"'");
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
