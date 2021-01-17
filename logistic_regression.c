//
//  logistic_regression.c
//  model
//
//  Created by Mac on 13/12/20.
//  Copyright © 2020 Mac. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"

/* initialize model */
logistic_regression *new_model(int n, double b, double tolerance, int epoch)
{
    logistic_regression *model = malloc(sizeof(logistic_regression));

    model->N = n;
    model->w = malloc(sizeof(double) * n);
    model->b = b;
    model->tolerance = tolerance;
    model->epoch = epoch;

    /* initialize weights with random values from 0 to 9 */
    for (int i = 0; i < n; i++)
        model->w[i] = rand() % 10;

    model->norm = norm(model);

    return model;
}

/* calculate linear function f(x) 
    f(x) = b + w1*x1 + w2*x2 + ... + wr*xr */
double fx(logistic_regression *model, struct node *file1, struct node *file2)
{
    double sum = 0.0;

    /* sum weights*tfidf for first file */
    for (int i = 0; i < file1->number_of_words; i++)
    {
        /* only for words with tfidf value > 0 */
        if (file1->non_zero_values[i] != 0)
            sum = sum + model->w[file1->non_zero_values[i]] * file1->tf_idf[file1->non_zero_values[i]];
        else
            break;
    }

    /* sum weights*tfidf of second file */
    for (int i = 0; i < file2->number_of_words; i++)
    {
        if (file2->non_zero_values[i] != 0)
            sum = sum + model->w[file2->non_zero_values[i]] * file2->tf_idf[file2->non_zero_values[i]];
        else
            break;
    }

    /* add b to total sum */
    return sum + model->b;
}

/* derivative_error_function = (sigmoid(w^T * xi + b) - yi) * xj */
/* is called for every component */
double derivative_error_function(logistic_regression *model, double *tf_idf1, double *tf_idf2, int j, int y, double sum_px)
{
    double temp_j;

    /* first file case */
    if (j < model->N / 2)
        temp_j = tf_idf1[j];

    /* we are in the second file */
    else
        temp_j = tf_idf2[j - model->N / 2];

    /* sum_px = p(f(x)), y = real value of trainning pair */
    return ((sum_px - y) * temp_j);
}

/* calculate norm of vector w = (w1, w2, ... ,wn) */
double norm(logistic_regression *model)
{
    double new_norm = 0.0;

    for (int i = 0; i < model->N; i++)
        new_norm = new_norm + model->w[i] * model->w[i];

    return sqrt(new_norm);
}

/* main function that trains our model */
void test_model(HashTable *files, logistic_regression *model)
{
    int index1;
    int index2;

    struct node *temp;
    struct node *temp2;


    /* train mode "epoch" times */
    for (int ep = 0; ep < model->epoch; ++ep)
    {
        int count = 0;                   /* total number of test data */

        int number_of_correct_1=0;
        int number_of_correct_0=0;
        int count0=0;
        int count1=0;



        lnode_data *start; /* first node of our trainning data-nodes */

        double p; /* probability obtained by our model */
        start = test->start; /* indicates to first node of our test data-nodes */

        /* for every node */
        while (start != NULL)
        {
            /* find hash indexes */
            index1 = hash1(start->data->file1, files->size);
            index2 = hash1(start->data->file2, files->size);

            /* find nodes-json files */
            temp = find_key_RBtree(files->Trees[index1], start->data->file1);
            temp2 = find_key_RBtree(files->Trees[index2], start->data->file2);

            /* calculate probability */
            p = px(fx(model, temp, temp2));

            /* calculate correct answers */
            if (start->data->match_flag == 0)
                count0 = count0 +1;
            else
                count1 = count1 +1;


            if (p < 0.5 && start->data->match_flag == 0)
                number_of_correct_0++;
            else if (p >= 0.5 && start->data->match_flag == 1)
                number_of_correct_1++;

            /* update count and pointer */
            count++;
            start = start->next;
        }

        printf("Accuracy for all: %f %%\n", (double)(number_of_correct_1+number_of_correct_0) * 100 / (double)count);

         printf("Accuracy for 0: %f %%\n",(double)(number_of_correct_0)*100 / (double)count0  );

        printf("Accuracy for 1: %f %%\n", (double)(number_of_correct_1)*100 / (double)count1 );
        //printf("%d %d\n",count0 , count1);

    }
}


/* p(x) = 1 / (1 + e^-f(x)) */
double px(double fx_val)
{
    double exponential = exp(-fx_val);
    return 1.0 / (1.0 + exponential);
}

/* calls derivative_error_function() for every component */
double *nabla(logistic_regression *model, job* Job)
{
    double *nabla_array = malloc(sizeof(double) * model->N);
    memset(nabla_array, 0, sizeof(double) * model->N);

    lnode_data * temp = Job->data->start;
    
    while(temp!=NULL)
    {
       // printf("before px\n");
        /* sum = p(f(x)) */
        if (temp->data->file1_node==NULL || temp->data->file2_node==NULL){
            printf("%s %s\n",temp->data->file1,temp->data->file2);
        }
//            printf("oof\n");

        double sum = px(fx(model, temp->data->file1_node, temp->data->file2_node));
       // printf("after px\n");
        /* first file case */
        for (int j = 0; j < temp->data->file1_node->number_of_words; j++)
        {
            if (temp->data->file1_node->non_zero_values[j] >= global_total_words)
                printf("bigger than global %d %d\n", temp->data->file1_node->non_zero_values[j], global_total_words);
            
            /* calculate only tfidf values that are greater than zero */
            if (temp->data->file1_node->non_zero_values[j] != 0)
            {
                printf("%f\n",sum);
                nabla_array[temp->data->file1_node->non_zero_values[j]] += derivative_error_function(model, temp->data->file1_node->tf_idf, temp->data->file2_node->tf_idf,
                                                                    temp->data->file1_node->non_zero_values[j], temp->data->match_flag, sum);
            }
            else
                break;
        }
        
        /* second file case */
        for (int j = 0; j < temp->data->file2_node->number_of_words; j++)
        {
            if (temp->data->file2_node->non_zero_values[j] >= global_total_words)
                printf("bigger than global %d %d\n", temp->data->file2_node->non_zero_values[j], global_total_words);
            
            /* calculate only tfidf values that are greater than zero */
            if (temp->data->file2_node->non_zero_values[j] != 0)
            {printf("%f\n",sum);
                nabla_array[temp->data->file2_node->non_zero_values[j] + model->N / 2] +=
                derivative_error_function(model,  temp->data->file1_node->tf_idf, temp->data->file2_node->tf_idf, temp->data->file2_node->non_zero_values[j] + model->N / 2, temp->data->match_flag, sum);
            }
            else
                break;
        }

        temp = temp->next;
        
    }

  //  Job->w=nabla_array;
    return nabla_array;
}

/* absolute value */
double absolute(double val)
{
    if (val < 0.0)
        return -val;
    else
        return val;
}

/* w^t+1 = w^t - learning_rate * nabla() */

void calculate_optimal_weights(logistic_regression *model, double learning_rate,jobScheduler * scheduler)
{

    double * averageW = malloc(sizeof(double)* model->N);
    memset(averageW, 0, sizeof(double)* model->N);
    
    int count=0;
    for (int i=0;i<NUMBER_OF_THREADS;i++)
    {
        if(scheduler->Matrix_w[i]!=NULL)
            for (int j=0 ; j<model->N;j++)
            {
               // printf("%f\n",scheduler->Matrix_w[i][j]);
                averageW[j] = averageW[j]+ scheduler->Matrix_w[i][j];
            }
        else
            count++;
        
    }
    
    for (int j=0 ; j<model->N;j++)
    {

            averageW[j] = averageW[j]/(MINI_BATCH_M- (count*(MINI_BATCH_M/NUMBER_OF_THREADS))) ;
            model->w[j] = model->w[j] - averageW[j]*learning_rate;
    }
        
  
    

 
}

/* free allocated memory */
void destroy_model(logistic_regression *model)
{
    free(model->w);
    free(model);
}
