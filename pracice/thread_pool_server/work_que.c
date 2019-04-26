#include "work_que.h"
void queInit(pQue_t pq,int capacity)
{
	memset(pq,0,sizeof(Que_t));//为了有一天用到别处
	pq->queCapacity=capacity;
	pthread_mutex_init(&pq->mutex,NULL);
}

void queInsert(pQue_t pq,pNode_t pnew)
{
	if(NULL==pq->queTail)
	{
		pq->queHead=pnew;
		pq->queTail=pnew;
	}else{
		pq->queTail->pNext=pnew;
		pq->queTail=pnew;
	}
	pq->queSize++;
}

int queGet(pQue_t pq,pNode_t* ppcur)
{
	if(0==pq->queSize)
	{
		return -1;
	}else{
		*ppcur=pq->queHead;
		pq->queHead=pq->queHead->pNext;
		if(NULL==pq->queHead)
		{
			pq->queTail=NULL;
		}
		pq->queSize--;
		return 0;
	}
}
