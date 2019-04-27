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
#include <mysql/mysql.h>
#include <limits.h>
#define ARGS_CHECK(argc,val) {if(argc!=val) {printf("error args\n");return -1;}}
#define ERROR_PTHREAD_CHECK(ret_val,pthreadname) {if(ret_val!=0)  {printf("%s is error,error num=ret_val");return -1;}}
#define THREAD_ERROR_CHECK(ret,funcname) {if (ret!=0){printf("%s failed,%ld\n",funcname,ret);}}

#define	ERROR_CHECK(ret,retval,funcname) {if(retval==ret){perror(funcname);return -1;}}
#define BASE_PATH "/home/xuyouzhi/Netv1.2/server"

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
void displayFileTypel(mode_t st_mode,Train *t);
int lsFunction(char *argv,int new_fd);
int pwdFunction(int new_fd,char *user);
int cdFunc(char* user,char *path,char *homePath,char *Home);
int tranFile(int new_fd,char *fileName);
int recvFile(int new_fd);
int remove_dir(const char *dir);
//int readConf(char *path,char *ip,char *port);
int readConf(char *path,char *ip,char *port,int* threadNum,int* capacity);
int mkdirFunction(char *path);
int logFunction(int new_fd,char *user);
int my_insert(char *userName,char *salt,char *password);
int my_query(const char *ch,char *userName);
int path_insert(char *username,char* currentPath);
int path_query(char *username,char *currentPath);
 int path_update(char *username,char* currentPath);
#endif
