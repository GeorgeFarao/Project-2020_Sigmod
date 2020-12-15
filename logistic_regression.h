//
//  logistic_regression.h
//  model
//
//  Created by Mac on 13/12/20.
//  Copyright © 2020 Mac. All rights reserved.
//

#ifndef logistic_regression_h
#define logistic_regression_h

typedef struct logistic_regression
{
    int N;
    double *w;
    double b;
    double tolerance;
    double norm;
    int epox;

} logistic_regression;

logistic_regression *new_model(int n, double b, double tolerance, int epox);

double fx(logistic_regression *model, struct node *file1, struct node *file2);

double derivative_error_function(logistic_regression *model, double *tf_idf1, double *tf_idf2, int j, int y, double sum_px);

double norm(logistic_regression *model);

void train(HashTable *files, logistic_regression *model);

double *nabla(logistic_regression *model, struct node *file1, struct node *file2, int y);

double px(double fx_val);

void calculate_optimal_weights(logistic_regression *model, struct node *file1, struct node *file2, int y, double learning_rate);

double absolute(double val);

#endif /* logistic_regression_h */
