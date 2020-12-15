//
//  dataList.c
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

list_data *data;
list_data *test;
list_data *validation;

list_data *new_list_data(void)
{
    list_data *mylist = malloc(sizeof(list_data));
    mylist->start = NULL;
    mylist->end = NULL;
    mylist->size = 0;

    return mylist;
}

/* Creating new list node */
lnode_data *new_lnode_data(train_data *data)
{
    lnode_data *node_data = malloc(sizeof(lnode_data));
    node_data->data = data;

    node_data->next = NULL;

    return node_data;
}

/* Inserting list node to list */
void insert_lnode_data(list_data *list_data, lnode_data *lnodedata)
{
    if (list_data->start == NULL) /* Checking if list is empty */
    {
        list_data->start = lnodedata;
        list_data->end = lnodedata;
    }
    else
    {
        list_data->end->next = lnodedata;
        list_data->end = lnodedata;
    }

    list_data->size = list_data->size + 1; /* Updating size */
}

train_data *new_train_data(char *str1, char *str2, int match_val)
{
    train_data *temp = malloc(sizeof(train_data));
    temp->file1 = malloc(strlen(str1) + 1);
    strcpy(temp->file1,str1);
    temp->file2 = malloc(strlen(str2) + 1);
    strcpy(temp->file2,str2);
    temp->match_flag = match_val;

    return temp;
}
