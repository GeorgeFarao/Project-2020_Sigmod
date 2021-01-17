//
//  dataList.h
//  model
//
//  Created by Mac on 13/12/20.
//  Copyright © 2020 Mac. All rights reserved.
//

#ifndef dataList_h
#define dataList_h

typedef struct train_data
{
    char *file1;
    char *file2;
    struct node * file1_node;
    struct node * file2_node;
    
    int match_flag;
    

} train_data;

typedef struct lnode_data
{
    struct lnode_data *next;
    train_data *data;
} lnode_data;

typedef struct list_data
{
    lnode_data *start;
    lnode_data *end;
    int size;

} list_data;

extern list_data *data;
extern list_data *test;
extern list_data *validation;

list_data *new_list_data(void);
lnode_data *new_lnode_data(train_data *data);
void insert_lnode_data(list_data *list_data, lnode_data *lnodedata);
train_data *new_train_data(char *str1, char *str2, int match_val);
void delete_dataList(list_data *data);

#endif /* dataList_h */
