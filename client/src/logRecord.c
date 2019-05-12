#include "../include/func.h"

int logRecord(char *userName,char *recordStr)
{
    FILE *fp;
    char str[1000]={0};
    struct timeval tv;
    struct timezone tz;   
    struct tm *t;
    gettimeofday(&tv, &tz);
    t = localtime(&tv.tv_sec);
    sprintf(str,"%d-%d-%d %d:%d:%d\t%s\t%s", 1900+t->tm_year, 1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,userName,recordStr);
   //printf("%s\n",getcwd(NULL,0));
    chdir(PATH);
    if ((fp=fopen("log/log_record.txt","a"))==NULL)			//打开指定文件，如果文件不存在则新建该文件
	{
		printf("Open Failed.\n");
	}
	fprintf(fp,"%s\n",str);					//格式化写入文件（追加至文件末尾）
	fclose(fp);
}
