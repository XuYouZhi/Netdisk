#include "factory.h"
void* downLoadFile(void *p)
{
	//判断队列是否为空，为空就睡觉，不为空就干活
	pFactory_t pf=(pFactory_t)p;
	pQue_t pq=&pf->que;
	pNode_t pcur;
	int getSuccess;
	while(1)
	{
		pthread_mutex_lock(&pq->mutex);
		if(!pq->queSize)
		{
			pthread_cond_wait(&pf->cond,&pq->mutex);
		}
		getSuccess=queGet(pq,&pcur);
		pthread_mutex_unlock(&pq->mutex);
		if(!getSuccess)
		{
			tranFile(pcur->new_fd);
			free(pcur);
		}
	}
}

void factoryInit(pFactory_t pf,int threadNum,int capacity)
{
	memset(pf,0,sizeof(Factory_t));
	pf->pthid=(pthread_t*)calloc(threadNum,sizeof(pthread_t));
	pf->threadNum=threadNum;
	pthread_cond_init(&pf->cond,NULL);
	queInit(&pf->que,capacity);
}

void factoryStart(pFactory_t pf)
{
	int i;
	if(0==pf->startFlag)
	{
		for(i=0;i<pf->threadNum;i++)
		{
			pthread_create(pf->pthid+i,NULL,downLoadFile,pf);
		}
		pf->startFlag=1;
	}
}
