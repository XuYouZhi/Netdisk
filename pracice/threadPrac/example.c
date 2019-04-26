#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
void* myFunc(void* args)
{
    int i;
    for (i=0;i<50;++i)
    {
        printf("%d\n",i);
    }
    return NULL;
}
int main()
{
    pthread_t th1;
    pthread_t th2;
    pthread_create(&th1,NULL,myFunc,NULL);
    pthread_create(&th2,NULL,myFunc,NULL);
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    return 0;
}
