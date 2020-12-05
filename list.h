/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#ifndef list_h
#define list_h



typedef struct lnode {
    struct lnode * next;
    char * json_name;
}lnode;





typedef struct list {
    lnode * start;
    lnode * end;
    struct RBTree * different_cliques;
    
    int print_flag; /* 1 when list is being printed, else 0 */
    int size; /* Size of list */
}list;

/////////////////////////////////////////   FUNCTIONS   /////////////////////////////////////////

list * new_list(void); /* Creating new list */

lnode * new_lnode(char * jsonName); /* Creating new list node */

void insert_lnode( list * list,  lnode * lnode); /* Inserting list node to list */

void delete_list( list * list); /* Deleting list */

void delete_list_node(list * List); /* Deleting first node of list */

void print_list( list * mylist); /* Printing matching json files - basic printing */










#endif /* list_h */


