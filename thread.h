//
//  thread.h
//  Project
//
//  Created by Mac on 16/1/21.
//  Copyright © 2021 Mac. All rights reserved.
//

#ifndef thread_h
#define thread_h



typedef struct job{
    int typeofJob;
    list_data *data;    //Data to train or test
    double * w;
    int N;
    
}job;


typedef struct lnode_thread {
    struct lnode_thread * next;
    job * Job;
    
}lnode_thread;





typedef struct queue {
    lnode_thread * start;
    lnode_thread * end;

    int size;
    int Removed_duplicates;
    
}queue;

typedef struct jobScheduler{
    queue * queue;
    pthread_t * threadIds;
    
    pthread_mutex_t mtx;
    pthread_cond_t readCond;
    pthread_cond_t writeCond;
    pthread_cond_t otherWriter;
    
    int readers;
    int writers;
    double ** Matrix_w;
    int index;
    int numWriters;
    int jobExists;

    int total_tested;
    int total_correct;
    int N;
    
    
    
}jobScheduler;






queue * new_queue(void); /* Creating new list */

lnode_thread * new_lnode_thread(job * Job); /* Creating new list node */

void insert_lnode_thread( queue * Queue,  lnode_thread * Node); /* Inserting list node to list */

void delete_queue( queue * Queue); /* Deleting list */

void delete_queue_node(queue * Queue); /* Deleting first node of list */

job * pop (queue * Queue);


jobScheduler * initialize_scheduler(logistic_regression * model);

void  submit_job(jobScheduler * scheduler,job * Job);
double *nabla(logistic_regression *model, job* Job);


/* for every component calculates w^t+1 = w^t - learning_rate * nabla() */
void calculate_optimal_weights(logistic_regression *model, double learning_rate,jobScheduler * scheduler);

void *Writer(void *modl);
void Reader(logistic_regression * model,double learning_rate , HashTable * new_table );
void CreateJobs(int flag);
int test_model(job *Job, logistic_regression *model,int *total_checked);
extern jobScheduler * scheduler;



#endif /* thread_h */
