/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"

/* Creating new list */
list * new_list(void)
{
    list * mylist = malloc(sizeof( list));
    mylist->start = NULL;
    mylist->end = NULL;
    mylist->print_flag = 0;
    mylist->size=0;
    mylist->Removed_duplicates=0;
    
    mylist->different_cliques = NULL;
    mylist->printed_different_cliques = NULL;
    
    
    
    return mylist;
}


/* Creating new list node */
lnode * new_lnode(char * jsonName)
{
    lnode * node = malloc(sizeof(lnode));
    node->json_name = malloc(strlen(jsonName) +1);
    strcpy(node->json_name,jsonName);
    
    node->next = NULL;
    node->file_node=NULL;
    
    return node;
}


/* Inserting list node to list */
void insert_lnode( list* list,  lnode* lnode)
{
    if(list->start == NULL) /* Checking if list is empty */
    {
        list->start = lnode;
        list->end = lnode;
    }
    else
    {
        list->end->next = lnode;
        list->end = lnode;
    }
    
    list->size = list->size + 1; /* Updating size */

}


/* Deleting list */
void delete_list( list * list)
{

    if (list->start == NULL) /* Checking if list is empty */
    {
        return;
    }
    else
    {
         lnode *prev = list->start; 
        
        while(list->start != NULL) /* Free allocated memory while moving prev and start pointers */
        {
            list->start = list->start->next; /* Move list->start to next node */
            free(prev->json_name);
            free(prev);
            prev = list->start; /* Prev indicated to previous state of list->start */
        }
    }
    
    list->start = list->end = NULL;
    list->size = -1;      /* So other nodes indicating to the list it's being deleted */
}


/* Deleting first node of list */
void delete_list_node(list * List) 
{
    struct lnode * tmp = List->start;
    List->start = List->start->next; /* Updating list->start */
    free(tmp->json_name);
    free(tmp);
    
    List->size--; /* Updating size */
    

    
    

}



/* Printing matching json files - basic printing */
void print_list(list * mylist , HashTable * files)
{
   
    FILE * fp = fopen("output.csv", "a");    
    lnode * temp = mylist->start;
    lnode * temp_next;
    int index1;
    int index2;
    struct node * temp_node1;
    struct node * temp_node2;
    
    /* Main loop */
    /* Printing basic node, and all matching nodes, that are after this node in the list */
    /* Move basic node so corresponding pointer indicates to the next node */
    /* Continue till basic node is null */
    int cnt=rand() %6;
    
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
            
            train_data * temp_data = new_train_data(temp->json_name, temp_next->json_name , 1);
            temp_data->file1_node = temp_node1;
            temp_data->file2_node = temp_node2;
            
            lnode_data * node_data =new_lnode_data( temp_data);

            if (cnt==5){
                insert_lnode_data(test,node_data);
              //  cnt =0 ;
            }
            else if(cnt==10){
               insert_lnode_data(validation,node_data);
                cnt=0;
            }
            else
                insert_lnode_data (data,node_data);
            cnt++;
            
            fprintf(fp, "%s, %s\n", temp->json_name , temp_next->json_name);
            temp_next = temp_next->next;
        }
        
        temp=temp->next;
    }
    
    mylist->print_flag = 1; /* So other nodes pointing to list know current list is being printed */
    fclose(fp);
}




void print_two_lists(list * mylist1 ,list * mylist2, HashTable * files)
{
    FILE * fp = fopen("different.csv", "a");

    lnode * temp = mylist1->start;
    lnode * temp_next ;
    int cnt= rand()%6;
    int index1;
    int index2;
    struct node * temp_node1;
    struct node * temp_node2;

    while (temp != NULL)
    {
        temp_next = mylist2->start;

        while (temp_next != NULL)
        {
            //printf( "second here %s, %s\n", temp->json_name , temp_next->json_name);
            /* find hash indexes */
            index1 = hash1(temp->json_name, files->size);
            index2 = hash1(temp_next->json_name, files->size);

            /* find nodes-json files */
            temp_node1 = find_key_RBtree(files->Trees[index1], temp->json_name);
            temp_node2 = find_key_RBtree(files->Trees[index2], temp_next->json_name);

            train_data * temp_data = new_train_data(temp->json_name, temp_next->json_name , 0);
            temp_data->file1_node = temp_node1;
            temp_data->file2_node = temp_node2;
            lnode_data * node_data =new_lnode_data( temp_data);

            if (cnt==5){
                //printf("OK\n");
                insert_lnode_data(test,node_data);
               // cnt = 0;

            }
           else if(cnt==10){
               insert_lnode_data(validation,node_data);
                cnt=0;
            }
            else
                insert_lnode_data (data,node_data);
            cnt++;

            
            
            fprintf(fp, "%s, %s\n", temp->json_name , temp_next->json_name);
            temp_next = temp_next->next;
        }

            temp=temp->next;
    }

    fclose(fp);

}
    



void create_validation_list(list * mylist )
{
    int i=0;
    lnode * temp = mylist->start;
    lnode * temp_next;
    int index1;
    int index2;
    struct node * temp_node1;
    struct node * temp_node2;
    
    /* Main loop */
    /* Printing basic node, and all matching nodes, that are after this node in the list */
    /* Move basic node so corresponding pointer indicates to the next node */
    /* Continue till basic node is null */
    
    while (temp->next != NULL)
    {
        temp_next = temp->next;
        
        while (temp_next != NULL)
        {
            if(i%500==0)
            {            /* find nodes-json files */
                temp_node1 = temp->file_node;
                temp_node2 =  temp_next->file_node;
        
                train_data * temp_data = new_train_data(temp->json_name, temp_next->json_name , 1);
                temp_data->file1_node = temp_node1;
                temp_data->file2_node = temp_node2;
        
                lnode_data * node_data =new_lnode_data( temp_data);
                insert_lnode_data(validation,node_data);
                if(validation->size%5000==0)
                    printf("size %d\n",validation->size);
                if(i>=VALIDATION_MAX_SIZE)
                    return;
            }
            temp_next = temp_next->next;
            i++;
        }
        
        temp=temp->next;
    }
    
    mylist->print_flag = 1; /* So other nodes pointing to list know current list is being printed */
}

void printval()
{
    lnode_data * temp = validation->start;
    while(temp!=NULL)
    {
        printf("%s %s \n",temp->data->file1,temp->data->file2);
        temp=temp->next;
    }
    
}
