#ifndef __FUNC_H__
#define __FUNC_H__
#include <sys/uio.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/msg.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <crypt.h>
#include <shadow.h>
#include <unistd.h>
#include<openssl/md5.h>
#define ARGS_CHECK(argc,val) {if(argc!=val) {printf("error args\n");return -1;}}
#define ERROR_PTHREAD_CHECK(ret_val,pthreadname) {if(ret_val!=0)  {printf("%s is error,error num=ret_val");return -1;}}
#define THREAD_ERROR_CHECK(ret,funcname) {if (ret!=0){printf("%s failed,%ld\n",funcname,ret);}}
#define	ERROR_CHECK(ret,retval,funcname) {if(retval==ret){perror(funcname);return -1;}}


typedef struct{
    pid_t pid;
    int fd;
    short busy;
}Data;

typedef struct{
    int datalen;
    char buf[1000];
}Train;

int makeChild(Data*,int);
int childHandle(int);
int tcpInit(int*,char*,char*);
int sendFd(int,int);
int recvFd(int,int *);
int sendCycle(int,char*,int);
int recvCycle(int,char*,int);
int recvFile(int socketFd,int flag);
int tranFile(int socketFd,char *fileName);
void getsalt(char *salt,char* passwd);
int logFunction(char  *argv);
int logRecord(char *userName,char *recordStr);
char* logFunction1(int socketFd);
#endif
