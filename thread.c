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
    queue * temp_queue = malloc(sizeof( queue));
    temp_queue->start = NULL;
    temp_queue->end = NULL;
    temp_queue->size=0;
    temp_queue->Removed_duplicates=0;
    
    
    return temp_queue;
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
//    if(Queue->size==0)
//        scheduler->jobExists=0;
    
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
    scheduler->numWriters = 0;
    scheduler->Matrix_w=malloc(sizeof(double *)* (NUMBER_OF_THREADS) );
    scheduler->N= model->N;
    scheduler->index=0;
    scheduler->jobExists = 0;
    scheduler->total_tested = 0;
    scheduler->total_correct = 0;
    
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



void Reader(logistic_regression * model,double learning_rate , HashTable * new_table )        //na sbhsoume to scheduler
{
    int i =2;
    HashTable * original_table = new_table;
    
    int iterations = data->size/MINI_BATCH_M;
    //iterations--;

   if(data->size % MINI_BATCH_M!=0)
        iterations++;
    
    int mod_res= iterations % NUMBER_OF_THREADS;
    if (mod_res !=0 )
        iterations+= mod_res;
    iterations++;
    iterations = iterations*model->epoch;

    while(iterations )
    {
        printf("Reader %d\n",iterations);
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->writers >0 || scheduler->index<(NUMBER_OF_THREADS)  )
            pthread_cond_wait(&scheduler->readCond, &scheduler->mtx);
        
        
        scheduler->readers = scheduler->readers +1;
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section
        //Calculate average and find new W
        calculate_optimal_weights(model, learning_rate, scheduler);
        for (int j = 0; j < NUMBER_OF_THREADS; ++j) {
            if (scheduler->Matrix_w[j]);
                free(scheduler->Matrix_w[j]);
        }
        
        scheduler->index=0;
        scheduler->numWriters=0;
        if(iterations ==1)
            scheduler->jobExists=0;
        
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->readers = scheduler->readers-1;
        pthread_cond_signal(&scheduler->writeCond);
        pthread_mutex_unlock(&scheduler->mtx);
        iterations--;
    }

    scheduler->index =NUMBER_OF_THREADS;
    //printf("%d\n",scheduler->jobExists);
    while(1)
    {
       // printf("to x einai %d, index %d\n",iterations, scheduler->index);
        pthread_mutex_lock(&scheduler->mtx);
        //printf("here %d %d %d \n",scheduler->writers,scheduler->index,scheduler->jobExists);
        while(scheduler->writers >0 || scheduler->index <(NUMBER_OF_THREADS)  )
            pthread_cond_wait(&scheduler->readCond, &scheduler->mtx);
        scheduler->readers = scheduler->readers +1;
        pthread_mutex_unlock(&scheduler->mtx);

        //printf("jobExists %d, q size %d\n",scheduler->jobExists,scheduler->queue->size);
        if(scheduler->jobExists == 0)
        {
            if(i!=0)
            {
                printf("douleia\n");
                new_table =CloneTable(original_table);
                validation_to_train = new_list_data();
                test_validation(new_table, model);
                CreateJobs(2);
                scheduler->jobExists=1;
                i--;
                
            }
            else
            {
                printf("katastrofh\n");
                scheduler->jobExists = 0;
                break;
            }
        }
        else
        {
            //printf("calculate opt\n");
            calculate_optimal_weights(model, learning_rate, scheduler);
            //printf("calculate opt meta\n");

            if(scheduler->queue->size==0) {
                scheduler->jobExists = 0;
                scheduler->index =NUMBER_OF_THREADS;

            }

        }
        if(scheduler->queue->size!=0)
            scheduler->index =0;
        scheduler->numWriters=0;
        
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->readers = scheduler->readers-1;
        pthread_cond_signal(&scheduler->writeCond);
        pthread_mutex_unlock(&scheduler->mtx);
        
        iterations--;
    }

    CreateJobs(3);
    scheduler->jobExists=1;
    scheduler->numWriters=0;
    scheduler->index=0;
    
    pthread_mutex_lock(&scheduler->mtx);
    scheduler->readers = scheduler->readers-1;
    pthread_cond_signal(&scheduler->writeCond);
    pthread_mutex_unlock(&scheduler->mtx);

    pthread_mutex_lock(&scheduler->mtx);
    printf("here %d %d %d \n",scheduler->writers,scheduler->index,scheduler->jobExists);
    while(scheduler->writers >0 || scheduler->index <(NUMBER_OF_THREADS)  )
        pthread_cond_wait(&scheduler->readCond, &scheduler->mtx);
    scheduler->readers = scheduler->readers +1;
    pthread_mutex_unlock(&scheduler->mtx);
    printf("Test Accuracy %d, %d\n", scheduler->total_tested, scheduler->total_correct);
    printf("Accuracy : %f %%\n", (double)(scheduler->total_correct) * 100 / (double)scheduler->total_tested);

    pthread_mutex_lock(&scheduler->mtx);
    scheduler->readers = scheduler->readers-1;
    pthread_cond_signal(&scheduler->writeCond);
    pthread_mutex_unlock(&scheduler->mtx);
    printf("exit reader\n");
}




void CreateJobs(int flag)
{
    lnode_data * temp;
    
    if(flag ==1)
    {
        scheduler->jobExists=1;
        temp = data->start;
    }
    else if (flag==2)
        temp = validation_to_train->start;
    else if (flag==3)
        temp = test->start;
    
    int i=0;
    job * tempJob = NULL;
    if (flag ==1 || flag == 2) {
        int numberofjobs = 0;
        while (temp != NULL) {
            if (i == 0 || i % (MINI_BATCH_M / NUMBER_OF_THREADS) == 0) {
                tempJob = malloc(sizeof(job));
                tempJob->data = new_list_data();
                tempJob->typeofJob = TRAINING;
            }

            lnode_data *temp_node = new_lnode_data(temp->data);
            insert_lnode_data(tempJob->data, temp_node);

            if (i % ((MINI_BATCH_M / NUMBER_OF_THREADS) - 1) == 0 || temp->next == NULL) {
                submit_job(scheduler, tempJob);
                numberofjobs++;
            }

            temp = temp->next;
            i++;
        }

        while (numberofjobs % NUMBER_OF_THREADS != 0) {
            tempJob = malloc(sizeof(job));
            tempJob->typeofJob = NOJOB;
            submit_job(scheduler, tempJob);
            numberofjobs++;

        }
    }
    else if (flag == 3){
        job ** Job= malloc(sizeof(job *)*NUMBER_OF_THREADS);
        for (int i=0;i<NUMBER_OF_THREADS;i++){
            Job[i]= malloc(sizeof(job));
            Job[i]->data=new_list_data();
            Job[i]->typeofJob= TESTING;
        }
        int indexjob=0;
        while (temp!=NULL){
            lnode_data *temp_node = new_lnode_data(temp->data);
            insert_lnode_data(Job[indexjob]->data, temp_node);
            indexjob++;
            if (indexjob == NUMBER_OF_THREADS)
                indexjob=0;
            temp= temp->next;
        }
        for (int i=0; i<NUMBER_OF_THREADS;i++){
            submit_job(scheduler, Job[i]);
        }
    }

    //printf("numberofjobs %d \n",numberofjobs);
    
    
}



void * Writer(void *modl)
{
    logistic_regression * model = (logistic_regression *)modl;
    
    int iterations = data->size/MINI_BATCH_M;
    if(data->size % MINI_BATCH_M!=0)
        iterations++;
    
    while (iterations % NUMBER_OF_THREADS !=0 )
        iterations++;
    
    iterations = iterations *model->epoch;
    while(1)
    {
        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0  ||scheduler->numWriters>=NUMBER_OF_THREADS || scheduler->jobExists==0 )
            pthread_cond_wait(&scheduler->writeCond, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        scheduler->numWriters =scheduler->numWriters+1;
        
        pthread_mutex_unlock(&scheduler->mtx);
        //Critical Section
       // printf("Poping a job %d\n",iterations);
        job * Job=pop(scheduler->queue);

        //Quit critical Section
        pthread_mutex_lock(&scheduler->mtx);
        scheduler->writers=0;
        
        if(scheduler->index<NUMBER_OF_THREADS)
            pthread_cond_signal(&scheduler->writeCond);
        pthread_mutex_unlock(&scheduler->mtx);
        
        //DO some work
        int total_checked;
        int num_of_correct;
        printf("%p\n", Job);
        if(Job->typeofJob != NOJOB)
        {
            if (Job->typeofJob == TRAINING)
                Job->w=nabla(model, Job);
            else if(Job->typeofJob == TESTING)
                num_of_correct = test_model(Job,model, &total_checked);
        }
//        if(Job->typeofJob == TERMINATE)
//            break ;

        //Enter critical section
        pthread_mutex_lock(&scheduler->mtx);
        while(scheduler->readers>0 || scheduler->writers>0)
            pthread_cond_wait(&scheduler->otherWriter, &scheduler->mtx);
        scheduler->writers = scheduler->writers+1;
        pthread_mutex_unlock(&scheduler->mtx);


        //Critical Section
        //printf("second crit %d\n",iterations);
        if(Job->typeofJob != NOJOB) {
            if (Job->typeofJob == TRAINING)
                scheduler->Matrix_w[scheduler->index] = Job->w;
            else if(Job->typeofJob == TESTING)
            {
                printf("Tested\n");
                scheduler->total_correct += num_of_correct;
                scheduler->total_tested += total_checked;
            }
        }
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
        int type=Job->typeofJob;

        delete_dataList(Job->data);
        free(Job);
        if (type == TESTING)
            break;
    }
    printf("writer exit\n");
    return NULL;
}




