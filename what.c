#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

typedef struct jobScheduler{
    pthread_t * threadIds;
    
    pthread_mutex_t mtx;
    pthread_cond_t readCond;
    pthread_cond_t writeCond;
    pthread_cond_t otherWriter;
    
    int readers;
    int writers;
    double ** Matrix_w;
    int index;
    int wwww;
    
    int numWriters;
    int N;
    
    
    
}jobScheduler;

int NUMBER_OF_THREADS=8;
jobScheduler * sss ;

jobScheduler * initialize_scheduler()
{
    jobScheduler * scheduler = malloc(sizeof(jobScheduler) );
    scheduler->wwww=0;
    
    pthread_mutex_init(&scheduler->mtx,0);
    pthread_cond_init(&scheduler->readCond, NULL);
    pthread_cond_init(&scheduler->writeCond, NULL);
    pthread_cond_init(&scheduler->otherWriter,NULL);

    scheduler->threadIds = malloc(sizeof(pthread_t)*8);
    
    scheduler->readers = 0;
    scheduler->writers = 0;
    scheduler->numWriters = 0;
    
    scheduler->N=100;    
    scheduler->Matrix_w=malloc(sizeof(double *)* (NUMBER_OF_THREADS) );
    scheduler->index=0;
    
    for (int i=0; i<(NUMBER_OF_THREADS); i++)
    {
        scheduler->Matrix_w[i]=NULL;
        //scheduler->Matrix_w[i] = malloc(sizeof(double)*model->N);
        //memset(scheduler->Matrix_w[i], 0, sizeof(double)*model->N);
    }
    return scheduler;
}







void Reader(jobScheduler *scheduler )        //na sbhsoume to scheduler
{
    int j;
    int  k=1000;
while(k>0){
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->writers>0  || scheduler->index<(NUMBER_OF_THREADS)  )
            pthread_cond_wait(&scheduler->readCond, &scheduler->mtx);
        
        scheduler->readers = scheduler->readers +1;
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section
        //Calculate average and find new W
       // calculate_optimal_weights(model, learning_rate, scheduler);
    
    
    
	for (int i=0; i<rand()%100000;i++)
		j=j+rand()%100;
	printf("Reader %d\n",j);            
	scheduler->index=0;
    scheduler->numWriters=0;
    scheduler->wwww++;
    

        pthread_mutex_lock(&scheduler->mtx);
        scheduler->readers = scheduler->readers-1;
        pthread_cond_signal(&scheduler->writeCond);
        pthread_mutex_unlock(&scheduler->mtx);
k--;
}
}


void *Writer(void *arg)
{
int k=1000;
jobScheduler * scheduler=sss;

while(k>0){




        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0 || scheduler->numWriters>=NUMBER_OF_THREADS)
            pthread_cond_wait(&scheduler->writeCond, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        scheduler->numWriters = scheduler->numWriters+1;
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section

    
    
        //Quit critical Section
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->writers=0;


        if(scheduler->index>=NUMBER_OF_THREADS)
            ;
        else
            pthread_cond_signal(&scheduler->writeCond);

        pthread_mutex_unlock(&scheduler->mtx);


        //DO some work
	int j=0;
        for (int i=0 ;i<rand()%1000000000;i++)
        j=j+rand()%100;
        printf("Writer %d %d\n",j,scheduler->wwww);



        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0)
            pthread_cond_wait(&scheduler->otherWriter, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        pthread_mutex_unlock(&scheduler->mtx);


        //Critical Section
        scheduler->index++;

        //Quit critical Section
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->writers=0;
        if(scheduler->index>=NUMBER_OF_THREADS)
            pthread_cond_signal(&scheduler->readCond);
        else
            pthread_cond_signal(&scheduler->otherWriter);
        pthread_mutex_unlock(&scheduler->mtx);
k--;
}
int * i= malloc(1);
return i;
}


int main()
{


srand(100);
sss= initialize_scheduler();


for (int i=0;i<  NUMBER_OF_THREADS  ;i++)
{
  pthread_create(&sss->threadIds[i], NULL, Writer,NULL);
}

Reader(sss);

}
