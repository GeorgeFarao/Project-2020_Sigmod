//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#ifndef list_h
#define list_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct lnode {
    struct lnode * next;
    char * json_name;
}lnode;

typedef struct list {
    lnode * start;
    lnode * end;
    int print_flag;
    int size;
    
}list;


 list * new_list(void);

lnode * new_lnode(char * jsonName);

void insert_lnode( list * list,  lnode * lnode);

int delete_key_list( list * list, char * key);

void delete_list( list * list);

void print_list( list * mylist);

// Returns pointer to patient or NULL if doesn't exist
// patientRecord * find_patient( list * list, int recID);





#endif /* list_h */

