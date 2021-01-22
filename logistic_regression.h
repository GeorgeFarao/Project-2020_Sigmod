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
    int N;     /* number of weights == 2*(number of columns of tfidf array) */
    double *w; /* array of weights */
    double b;  /* constant used in linear function f(x) */
    double tolerance;
    double norm;
    int epoch; /* number of epoches */

} logistic_regression;



typedef struct validation_fix_weight{
    double prob;
    struct node * file1;
    struct node * file2;
    
    
}validation_fix_weight;



/* initialize model */
logistic_regression *new_model(int n, double b, double tolerance, int epoch);

/* calculate linear function f(x) 
    f(x) = b + w1*x1 + w2*x2 + ... + wr*xr */
double fx(logistic_regression *model, struct node *file1, struct node *file2);

/* derivative_error_function = (p(f(x)) - yi) * xj */
double derivative_error_function(logistic_regression *model, double *tf_idf1, double *tf_idf2, int j, int y, double sum_px);

/* calculate norm of vector w = (w1, w2, ... ,wn) */
double norm(logistic_regression *model);

/* main function that trains our model */


/* calls derivative_error_function() for every component */

/* p(x) = 1 / (1 + e^-f(x)) */
double px(double fx_val);


/* absolute value */
double absolute(double val);
int absolute_int(int val);
/* free allocated memory */
void destroy_model(logistic_regression *model);

void postorder_findCliques_conflicts(struct RBTree *T, struct node *node, HashTable * files ,logistic_regression * model);
void fixConflicts(HashTable * files , list * clique, logistic_regression * model);
void find_conflicts(HashTable * table,logistic_regression * model );
void test_validation(HashTable *files, logistic_regression *model);

void postorder_findCliques_conflicts_test(struct RBTree *T, struct node *node, HashTable * files ,logistic_regression * model, int * flag);
int find_conflicts_test(HashTable * table,logistic_regression * model );




#endif /* logistic_regression_h */
