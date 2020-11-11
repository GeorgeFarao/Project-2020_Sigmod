//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#include "list.h"

list * new_list(void)
{
    list * mylist = malloc(sizeof( list));
    mylist->start = NULL;
    mylist->end = NULL;
    mylist->print_flag = 0;
    mylist->size=0;    
    
    return mylist;
}

lnode * new_lnode(char * jsonName)
{
    lnode * node = malloc(sizeof(lnode));
    node->json_name = malloc(strlen(jsonName) +1);
    strcpy(node->json_name,jsonName);
    
    node->next = NULL;
    
    return node;
}

void insert_lnode( list* list,  lnode* lnode)
{
    if(list->start == NULL)
    {
        list->start = lnode;
        list->end = lnode;
    }
    else
    {
        list->end->next = lnode;
        list->end = lnode;
    }
    
    list->size = list->size + 1;

}

/*
int delete_key_list(struct list * list, char * key) //returns 1 if key was found and deleted in the list, else 0
{
    if(strcmp(list->start->patient->entryDate, key) == 0) //if key is in list->start
    {
        
        if(list->start == list->end) //if only one node in list
            list->end = NULL;
        
        struct lnode *to_del = list->start;
        list->start = list->start->next; //update list->start
        deletePatientRecord(to_del->patient);
        free(to_del);
        
        return 1;
    }
    else
    {
        struct lnode *temp = list->start;
        
        while(temp->next != NULL)
        {
            if(strcmp(temp->next->patient->entryDate, key) == 0)
            {
                if(list->end == temp->next) //if key in list->end update list->end
                    list->end = temp;
                
                struct lnode *to_del = temp->next;
                temp->next = temp->next->next; //update temp->next
                deletePatientRecord(to_del->patient);
                free(to_del);
                
                return 1;
            }
            else
                temp = temp->next;
        }
        
        return 0;
    }
    
}
*/
void delete_list( list * list)
{
    if (list->start == NULL)
    {
        printf("List is empty (delete_list)\n");
        return;
    }
    else
    {
         lnode *prev = list->start;
        
        while(list->start != NULL)
        {
            list->start = list->start->next;
            free(prev->json_name);
            free(prev);
            prev = list->start;
        }
    }
    
    list->start = list->end = NULL;
    
}
void print_list(list * mylist)
{
   
    FILE * fp = fopen("output.txt", "a");    
    lnode * temp = mylist->start;
    lnode * temp_next;
    
    while ( temp->next !=NULL)
    {
        
        temp_next=temp->next;
        while (temp_next!=NULL)
        {
            fprintf(fp, "%s, %s \n", temp->json_name , temp_next->json_name);
            temp_next = temp_next->next;
        }
        
        temp=temp->next;
    }
    
    mylist->print_flag = 1;
    fclose(fp);
    
}

/*
patientRecord * find_patient(struct list * list, int recID)
{
    if (list->start == NULL)
    {
        //        printf("List is empty (find_patient)\n");
        return NULL;
    }
    else
    {
        struct lnode *temp = list->start;
        
        while(temp != NULL)
        {
            if(temp->patient->recordID == recID)
                return temp->patient;
            else
                temp = temp->next;
        }
    }
    
    return NULL;
}
*/

