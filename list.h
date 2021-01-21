/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#ifndef list_h
#define list_h



typedef struct lnode {
    struct lnode * next;
    char * json_name;
    struct node * file_node;

}lnode;





typedef struct list {
    lnode * start;
    lnode * end;
    struct RBTree * different_cliques;
    struct RBTree * printed_different_cliques;
    
    
    int print_flag; /* 1 when list is being printed, else 0 */
    int size; /* Size of list */
    int Removed_duplicates;
    
}list;

/////////////////////////////////////////   FUNCTIONS   /////////////////////////////////////////

list * new_list(void); /* Creating new list */

lnode * new_lnode(char * jsonName); /* Creating new list node */

void insert_lnode( list * list,  lnode * lnode); /* Inserting list node to list */

void delete_list( list * list); /* Deleting list */

void delete_list_node(list * List); /* Deleting first node of list */


void create_validation_list(list * mylist );


void printval();






#endif /* list_h */


