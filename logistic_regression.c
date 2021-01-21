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


void test_validation(HashTable *files, logistic_regression *model)
{
    int index1;
    int index2;

    struct node *temp;
    struct node *temp2;

    /* train model */

    lnode_data *start; /* first node of our trainning data-nodes */

    double p; /* probability obtained by our model */
    start = validation->start; /* indicates to first node of our test data-nodes */

    /* for every node */

    while (start != NULL)
    {
        temp= start->data->file1_node;
        temp2 = start->data->file2_node;

        /* calculate probability */
        p = px(fx(model, temp, temp2));

        if (p < 0.5 ) {
            match_different_products(files, temp->key, temp2->key);
        }
        else if (p >= 0.5) {
            match_same_products(files, temp->key, temp2->key);
        }

        /* update pointer */
        start = start->next;
    }
    remove_duplicates(files);
    find_conflicts(files, model);

}

/* main function that trains our model */
int test_model(job * Job, logistic_regression *model, int *total_checked)
{
    int index1;
    int index2;

    struct node *temp;
    struct node *temp2;

    /* train model */

        int count = 0;                   /* total number of test data */

        int number_of_correct_1=0;
        int number_of_correct_0=0;
        int count0=0;
        int count1=0;

        lnode_data *start; /* first node of our trainning data-nodes */

        double p; /* probability obtained by our model */
        start = Job->data->start; /* indicates to first node of our test data-nodes */

        /* for every node */
        while (start != NULL)
        {

            /* find nodes-json files */
            temp = start->data->file1_node;
            temp2 = start->data->file2_node;

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

        *total_checked= count;
    return number_of_correct_1+number_of_correct_0;

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

        /* sum = p(f(x)) */
        if (temp->data->file1_node==NULL || temp->data->file2_node==NULL){
            printf("%s %s\n",temp->data->file1,temp->data->file2);
        }

        double sum = px(fx(model, temp->data->file1_node, temp->data->file2_node));
        /* first file case */
        for (int j = 0; j < temp->data->file1_node->number_of_words; j++)
        {
            if (temp->data->file1_node->non_zero_values[j] >= global_total_words)
                printf("bigger than global %d %d\n", temp->data->file1_node->non_zero_values[j], global_total_words);
            
            /* calculate only tfidf values that are greater than zero */
            if (temp->data->file1_node->non_zero_values[j] != 0)
            {
                
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
            {
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

int absolute_int(int val)
{
    if (val < 0)
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
                averageW[j] = averageW[j]+ scheduler->Matrix_w[i][j];
            }
        else
            count++;
        
    }
    
    for (int j=0 ; j<model->N;j++)
    {

            model->w[j] = model->w[j] - averageW[j]*learning_rate;
    }
    free(averageW);
}

/* free allocated memory */
void destroy_model(logistic_regression *model)
{
    free(model->w);
    free(model);
}




void fixConflicts(HashTable * files , list * clique, logistic_regression * model)
{
    
    double total_prob =0.0;
    int MO =0;

    validation_fix_weight ** probs = malloc(sizeof(validation_fix_weight *) * clique->size);
    for(int i =0 ;i<clique->size; i++ )
    {
        probs[i] = malloc(sizeof(validation_fix_weight )*clique->size);
        for (int j=0; j<clique->size; j++)
            probs[i][j].prob =0;
        
        
    }

    int index1;
    int index2;
    
    struct node * temp_node1;
    struct node * temp_node2;
    
    lnode * temp=clique->start;
    lnode * temp_next;
    
    int i=0;
    int j=1;
    
    while (temp->next != NULL)
    {
        temp_next = temp->next;

        while (temp_next != NULL)
        {
            /* find hash indexes */
            index1 = hash1(temp->json_name, files->size);
            index2 = hash1(temp_next->json_name, files->size);
            
            /* find nodes-json files */
            temp_node1 = find_key_RBtree(files->Trees[index1], temp->json_name);
            temp_node2 = find_key_RBtree(files->Trees[index2], temp_next->json_name);
            temp_node1 = temp_node1->self_node;
            temp_node2 = temp_node2->self_node;
            
            probs[i][j].file1 = temp_node1;
            probs[i][j].file2 = temp_node2;
            
            probs[j][i].file1 = temp_node1;
            probs[j][i].file2 = temp_node2;
            
            probs[i][j].prob = px(fx(model, temp_node1, temp_node2));
            probs[j][i].prob = probs[i][j].prob;
            probs[i][i].prob = probs[i][i].prob+ probs[i][j].prob;
            probs[j][j].prob = probs[j][j].prob + probs[j][i].prob;
            
            total_prob = total_prob + probs[i][j].prob;
            MO++;
            
            temp_next = temp_next->next;
            j++;
        }
        
        temp=temp->next;
        i++;
        j=i+1;
    }
    
    
    total_prob = total_prob/(double)MO;
    if(total_prob > 0.5)
    {
        int i=0;
        int j=1;
        while (i!=clique->size-1)
        {
 
            while (j!=clique->size)
            {
                /* find hash indexes */
                if(probs[i][j].prob <0.5)
                {
                    train_data * temp_data = new_train_data(probs[i][j].file1->key, probs[i][j].file2->key , 1);
                    temp_data->file1_node = probs[i][j].file1;
                    temp_data->file2_node = probs[i][j].file2;

                    lnode_data * node_data =new_lnode_data( temp_data);
                    insert_lnode_data(validation_to_train,node_data);
                }
                
                j++;
            }
            i++;
            j=i+1;
        }
    }
    else
    {
        for(int i=0 ;i<clique->size;i++)
        {
            probs[i][i].prob = probs[i][i].prob/ (clique->size-1);
            if(probs[i][i].prob < 0.5)
            {
                for (int j=0; j<clique->size-1; j++)
                    if(j!=i)
                    {
                        train_data * temp_data = new_train_data(probs[i][j].file1->key, probs[i][j].file2->key , 0);
                        temp_data->file1_node = probs[i][j].file1;
                        temp_data->file2_node = probs[i][j].file2;

                        lnode_data * node_data =new_lnode_data( temp_data);
                        insert_lnode_data(validation_to_train,node_data);
                        
                    }
                
                
            }
            
        }
        
    }


    for (int k = 0; k < clique->size; ++k) {
        free(probs[k]);
    }
    free(probs);
}
