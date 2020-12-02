/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#include "list.h"


/* Creating new list */
list * new_list(void)
{
    list * mylist = malloc(sizeof( list));
    mylist->start = NULL;
    mylist->end = NULL;
    mylist->print_flag = 0;
    mylist->size=0;
    
    mylist->different_cliques = NULL;

    return mylist;
}


/* Creating new list node */
lnode * new_lnode(char * jsonName)
{
    lnode * node = malloc(sizeof(lnode));
    node->json_name = malloc(strlen(jsonName) +1);
    strcpy(node->json_name,jsonName);
    
    node->next = NULL;
    
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
void print_list(list * mylist)
{
   
    FILE * fp = fopen("output.csv", "a");    
    lnode * temp = mylist->start;
    lnode * temp_next;
    
    /* Main loop */
    /* Printing basic node, and all matching nodes, that are after this node in the list */
    /* Move basic node so corresponding pointer indicates to the next node */
    /* Continue till basic node is null */
    while (temp->next != NULL) 
    {        
        temp_next = temp->next;
        
        while (temp_next != NULL)
        {
            fprintf(fp, "%s, %s\n", temp->json_name , temp_next->json_name);
            temp_next = temp_next->next;
        }
        
        temp=temp->next;
    }
    
    mylist->print_flag = 1; /* So other nodes pointing to list know current list is being printed */
    fclose(fp);
}
