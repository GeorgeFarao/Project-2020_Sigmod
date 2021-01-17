//
//  thread.c
//  Project
//
//  Created by Mac on 16/1/21.
//  Copyright © 2021 Mac. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"




queue * new_queue(void) /* Creating new list */
{
    queue * queue = malloc(sizeof( queue));
    queue->start = NULL;
    queue->end = NULL;
    queue->size=0;
    queue->Removed_duplicates=0;
    
    
    return queue;
}


/* Creating new list node */
lnode_thread * new_lnode_thread(job * Job) /* Creating new list node */
{
    lnode_thread * node_thread = malloc(sizeof(lnode_thread));
    node_thread->Job = Job;
    
    node_thread->next = NULL;
    
    return node_thread;
}


/* Inserting list node to list */
void insert_lnode_thread( queue * Queue,  lnode_thread * Node)
{
    if(Queue->start == NULL) /* Checking if list is empty */
    {
        Queue->start = Node;
        Queue->end = Node;
    }
    else
    {
        Queue->end->next = Node;
        Queue->end = Node;
    }
    
    Queue->size = Queue->size + 1; /* Updating size */
    
}


/* Deleting list */

//void delete_queue( queue * Queue)
//{
//
//    if (Queue->start == NULL) /* Checking if list is empty */
//    {
//        return;
//    }
//    else
//    {
//        lnode *prev = list->start;
//
//        while(list->start != NULL) /* Free allocated memory while moving prev and start pointers */
//        {
//            list->start = list->start->next; /* Move list->start to next node */
//            free(prev->json_name);
//            free(prev);
//            prev = list->start; /* Prev indicated to previous state of list->start */
//        }
//    }
//
//    list->start = list->end = NULL;
//    list->size = -1;      /* So other nodes indicating to the list it's being deleted */
//}


/* Deleting first node of list */
void delete_queue_node(queue * Queue)
{
    lnode_thread * tmp = Queue->start;
    Queue->start = Queue->start->next; /* Updating list->start */
    
    free(tmp->Job->w);
    delete_dataList(tmp->Job->data);
    free(tmp);
    
    Queue->size--; /* Updating size */
    
    
    
    
    
}


job * pop (queue * Queue)
{
    
    if(Queue->start==NULL)
        return NULL;
    
    lnode_thread * tmp = Queue->start;
    Queue->start = Queue->start->next; /* Updating list->start */
    
    job * Job = tmp->Job;
    free(tmp);

    
    Queue->size--; /* Updating size */
    return Job;
}



jobScheduler * initialize_scheduler(logistic_regression * model)
{
    jobScheduler * scheduler = malloc(sizeof(jobScheduler) );
 
    scheduler->queue = new_queue();
    
    pthread_mutex_init(&scheduler->mtx,0);
    pthread_cond_init(&scheduler->readCond, NULL);
    pthread_cond_init(&scheduler->writeCond, NULL);
    pthread_cond_init(&scheduler->otherWriter, NULL);

    scheduler->threadIds = malloc(sizeof(pthread_t)*NUMBER_OF_THREADS);
    
    scheduler->readers = 0;
    scheduler->writers = 0;
    
    scheduler->Matrix_w=malloc(sizeof(double *)* (NUMBER_OF_THREADS) );
    scheduler->N= model->N;
    scheduler->index=0;
    
    for (int i=0; i<(NUMBER_OF_THREADS); i++)
    {
        scheduler->Matrix_w[i]=NULL;
        //scheduler->Matrix_w[i] = malloc(sizeof(double)*model->N);
        //memset(scheduler->Matrix_w[i], 0, sizeof(double)*model->N);
    }
    return scheduler;
}



void submit_job(jobScheduler * scheduler,job * Job)
{
    lnode_thread * node = new_lnode_thread(Job);
    insert_lnode_thread(scheduler->queue, node);
    
}


void Reader(jobScheduler *scheduler,logistic_regression * model,double learning_rate )        //na sbhsoume to scheduler
{

    int iterations = data->size/MINI_BATCH_M;
    if(data->size % MINI_BATCH_M!=0)
        iterations++;
    
    while(iterations)
    {
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->writers >0 || scheduler->index<(NUMBER_OF_THREADS)  )
            pthread_cond_wait(&scheduler->readCond, &scheduler->mtx);
        
        scheduler->readers = scheduler->readers +1;
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section
        //Calculate average and find new W
        calculate_optimal_weights(model, learning_rate, scheduler);
        
        scheduler->index=0;
        scheduler->numWriters=0;
        
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->readers = scheduler->readers-1;
        pthread_cond_signal(&scheduler->writeCond);
        pthread_mutex_unlock(&scheduler->mtx);
        iterations--;
    }
}

void CreateJobs(jobScheduler * scheduler)
{
    lnode_data * temp = data->start;
    int i=0;
    job * tempJob = NULL;
    
    
    while(temp!=NULL)
    {
        if(i==0 || i %(MINI_BATCH_M/NUMBER_OF_THREADS) ==0)
        {
            tempJob = malloc(sizeof(job));
            tempJob->data =new_list_data();
            tempJob->typeofJob =TRAINING;
        }
        
        lnode_data* temp_node=new_lnode_data(temp->data);
        insert_lnode_data(tempJob->data, temp_node);
        
        if(i%  ((MINI_BATCH_M/NUMBER_OF_THREADS)-1)  ==0 ||temp->next==NULL)
            submit_job(scheduler, tempJob);
        
        temp = temp->next;
        i++;
    }
    
}




void Writer(jobScheduler * scheduler,logistic_regression *model)
{
    
    int iterations = data->size/MINI_BATCH_M;
    if(data->size % MINI_BATCH_M!=0)
        iterations++;
    
    
    while(iterations)
    {
        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0  ||scheduler->numWriters>=NUMBER_OF_THREADS )
            pthread_cond_wait(&scheduler->writeCond, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        scheduler->numWriters =scheduler->numWriters+1;
        
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section
        job * Job=pop(scheduler->queue);

        //Quit critical Section
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->writers=0;
        
        if(scheduler->index<NUMBER_OF_THREADS)
            pthread_cond_signal(&scheduler->writeCond);
        
        pthread_mutex_unlock(&scheduler->mtx);
        
        //DO some work
        if(Job!=NULL)
            nabla(model, Job);

        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0)
            pthread_cond_wait(&scheduler->otherWriter, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        pthread_mutex_unlock(&scheduler->mtx);


        //Critical Section
        if(Job!=NULL)
            scheduler->Matrix_w[scheduler->index ] = Job->w;
        else
            scheduler->Matrix_w[scheduler->index ] =NULL;
        scheduler->index++;

        //Quit critical Section
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->writers=0;
        if(scheduler->index>=NUMBER_OF_THREADS)
            pthread_cond_signal(&scheduler->readCond);
        else
            pthread_cond_signal(&scheduler->otherWriter);
        pthread_mutex_unlock(&scheduler->mtx);
        
        iterations--;
    }

}




