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

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"

logistic_regression *new_model(int n, double b, double tolerance, int epox)
{
    logistic_regression *model = malloc(sizeof(logistic_regression));

    model->N = n;
    model->w = malloc(sizeof(double) * n);
    model->b = b;
    model->tolerance = tolerance;
    model->epox = epox;
    for (int i = 0; i < n; i++)
        model->w[i] = rand() % 10;

    model->norm = norm(model);

    return model;
}

double fx(logistic_regression *model, struct node *file1, struct node *file2)
{
    double sum = 0.0;
    for (int i = 0; i < file1->number_of_words; i++)
    {
        if (file1->non_zero_values[i] != 0)
            sum = sum + model->w[file1->non_zero_values[i]] * file1->tf_idf[file1->non_zero_values[i]];
        else
            break;
    }
    for (int i = 0; i < file2->number_of_words; i++)
    {
        if (file2->non_zero_values[i] != 0)
            sum = sum + model->w[file2->non_zero_values[i]] * file2->tf_idf[file2->non_zero_values[i]];
        else
            break;
    }
    return sum + model->b;
}

double derivative_error_function(logistic_regression *model, double *tf_idf1, double *tf_idf2, int j, int y, double sum_px)
{
    double temp_j;
    if (j < model->N / 2)
        temp_j = tf_idf1[j];
    else
        temp_j = tf_idf2[j - model->N / 2];

    return ((sum_px - y) * temp_j);
}

double norm(logistic_regression *model)
{
    double new_norm = 0.0;

    for (int i = 0; i < model->N; i++)
        new_norm = new_norm + model->w[i] * model->w[i];

    return sqrt(new_norm);
}

void train(HashTable *files, logistic_regression *model)
{
    int index1;
    int index2;

    struct node *temp;
    struct node *temp2;

    for (int ep = 0; ep < model->epox; ++ep)
    {
        int count = 0;
        printf("%d\n", data->size);
        lnode_data *start = data->start;

        for (int i = 0; i < data->size; i++)
        {
            index1 = hash1(start->data->file1, files->size);
            index2 = hash1(start->data->file2, files->size);

            temp = find_key_RBtree(files->Trees[index1], start->data->file1);
            temp2 = find_key_RBtree(files->Trees[index2], start->data->file2);

            calculate_optimal_weights(model, temp, temp2, start->data->match_flag, 0.01);
            start = start->next;
        }

        double p;
        int numOfCorrectAnswers = 0;
        printf("start2\n");
        start = test->start;
        while (start != NULL)
        {

            index1 = hash1(start->data->file1, files->size);
            index2 = hash1(start->data->file2, files->size);

            temp = find_key_RBtree(files->Trees[index1], start->data->file1);
            temp2 = find_key_RBtree(files->Trees[index2], start->data->file2);

            p = px(fx(model, temp, temp2));
            if (p < 0.5 && start->data->match_flag == 0)
                numOfCorrectAnswers++;
            else if (p >= 0.5 && start->data->match_flag == 1)
                numOfCorrectAnswers++;
            //printf("train:%f, real:%d\n",p,start->data->match_flag );
            count++;
            start = start->next;
        }

        printf("Accuracy: %f %%\n", (double)numOfCorrectAnswers * 100 / (double)count);
    }
}

double px(double fx_val)
{
    double exponential = exp(-fx_val);
    return 1.0 / (1.0 + exponential);
}

double *nabla(logistic_regression *model, struct node *file1, struct node *file2, int y)
{
    double *nabla_array = malloc(sizeof(double) * model->N);

    memset(nabla_array, 0, sizeof(double) * model->N);
    double sum = px(fx(model, file1, file2));
    // printf("start of nabla\n");
    for (int j = 0; j < file1->number_of_words; j++)
    {
        if (file1->non_zero_values[j] >= global_total_words)
            printf("bigger than global %d %d\n", file1->non_zero_values[j], global_total_words);
        if (file1->non_zero_values[j] != 0)
        {
            // printf("%d\n",file1->non_zero_values[j]);
            nabla_array[file1->non_zero_values[j]] = derivative_error_function(model, file1->tf_idf, file2->tf_idf,
                                                                               file1->non_zero_values[j], y, sum);
        }
        else
            break;
    }

    for (int j = 0; j < file2->number_of_words; j++)
    {
        if (file2->non_zero_values[j] >= global_total_words)
            printf("bigger than global %d %d\n", file2->non_zero_values[j], global_total_words);
        if (file2->non_zero_values[j] != 0)
        {
            nabla_array[file2->non_zero_values[j] + model->N / 2] = derivative_error_function(model, file1->tf_idf,
                                                                                              file2->tf_idf,
                                                                                              file2->non_zero_values[j] +
                                                                                                  model->N / 2,
                                                                                              y, sum);
        }
        else
            break;
    }

    return nabla_array;
}

double absolute(double val)
{
    if (val < 0.0)
        return -val;
    else
        return val;
}

void calculate_optimal_weights(logistic_regression *model, struct node *file1, struct node *file2, int y, double learning_rate)
{
    double *nabla_array = nabla(model, file1, file2, y);

    for (int j = 0; j < file1->number_of_words; j++)
    {
        // printf("first loop\n");
        if (file1->non_zero_values[j] != 0)
            model->w[file1->non_zero_values[j]] = model->w[file1->non_zero_values[j]] -
                                                  learning_rate * nabla_array[file1->non_zero_values[j]];

        else
            break;
    }

    for (int j = 0; j < file2->number_of_words; j++)
    {
        // printf("second loop\n");
        if (file2->non_zero_values[j] != 0)
            model->w[file2->non_zero_values[j] + model->N / 2] = model->w[file2->non_zero_values[j] + model->N / 2] -
                                                                 learning_rate * nabla_array[file2->non_zero_values[j] + model->N / 2];
        else
            break;
    }

    free(nabla_array);

    double new_norm = norm(model);

    if (absolute(new_norm - model->norm) < model->tolerance)
        ;

    model->norm = new_norm;
}
