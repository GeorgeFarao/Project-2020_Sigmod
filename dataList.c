//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

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

list_data *data;
list_data *test;
list_data *validation;
list_data * validation_to_train;
list * allfiles;

int conflicts;

jobScheduler * scheduler;


list_data *new_list_data(void)      //initializes new list data
{
    list_data *mylist = malloc(sizeof(list_data));
    mylist->start = NULL;
    mylist->end = NULL;
    mylist->size = 0;

    return mylist;
}

/* Creating new list node */
lnode_data *new_lnode_data(train_data *data)        //initializes a new list
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

train_data *new_train_data(char *str1, char *str2, int match_val)       //inserts new train data in the node
{
    train_data *temp = malloc(sizeof(train_data));
    temp->file1 = malloc(strlen(str1) + 1);             //copy first file's name
    strcpy(temp->file1,str1);
    temp->file2 = malloc(strlen(str2) + 1);             //copy second file's name
    strcpy(temp->file2,str2);
    temp->match_flag = match_val;

    return temp;
}

void delete_dataList(list_data *dataList)       // deletes a data list
{
    if (dataList->start == NULL) /* Checking if list is empty */
    {
        free(dataList);
        return;
    }
    else
    {
        lnode_data *prev = dataList->start;

        while(dataList->start != NULL) /* Free allocated memory while moving prev and start pointers */
        {
            dataList->start = dataList->start->next; /* Move list->start to next node */
            free(prev->data->file1);
            free(prev->data->file2);
            free(prev->data);
            free(prev);
            prev = dataList->start; /* Prev indicated to previous state of list->start */
        }
    }

    dataList->start = dataList->end = NULL;
    dataList->size = -1;      /* So other nodes indicating to the list it's being deleted */
    free(dataList);
}

void delete_dataList_val(list_data *dataList)       // similar to delete datalist but for differeent list
{
    if (dataList->start == NULL) /* Checking if list is empty */
    {
        free(dataList);
        return;
    }
    else
    {
        lnode_data *prev = dataList->start;

        while(dataList->start != NULL) /* Free allocated memory while moving prev and start pointers */
        {
            dataList->start = dataList->start->next; /* Move list->start to next node */
            free(prev->data->file1);
            free(prev->data->file2);
            free(prev);
            prev = dataList->start; /* Prev indicated to previous state of list->start */
        }
    }

    dataList->start = dataList->end = NULL;
    dataList->size = -1;      /* So other nodes indicating to the list it's being deleted */
    free(dataList);
}
