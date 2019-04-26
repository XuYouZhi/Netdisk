#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#include "tranfile.h"
typedef struct{
	pthread_t *pthid;//存储线程ID的起始地址
	int threadNum;//要创建的线程数目
	pthread_cond_t cond;//每个线程都要使用的条件变量
	Que_t que;//生产者，消费者操作的队列
	short startFlag;//工厂启动标志
}Factory_t,*pFactory_t;
void factoryInit(pFactory_t,int,int);
void factoryStart(pFactory_t);
int tcpInit(int *,char *,char*);
#endif
