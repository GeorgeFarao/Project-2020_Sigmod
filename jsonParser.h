/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#include "list.h"

typedef struct Hashtable * hashptr;

typedef struct json_node {
    char * category; /* Category name */
    list * values; /* List of possible values - Values of a category can be more than one */
    struct json_node * next;
} json_node;

/* Main json_list that holds info of product */
/* Number of nodes of the list are equal to the different number of categories of the product */
typedef struct json_list {
    json_node * start;
    json_node * end;
    int number_of_categories; 
} json_list;


/////////////////////////////////////////   FUNCTIONS   /////////////////////////////////////////

json_list * new_json_list (void); /* Creating main json list with product info */

json_node * new_json_node (char * category); /* New json node - New category to product */

void insert_json_list_node (json_list * jsonList, json_node * node); /* Insert json node - category to json list */

void add_category_value (json_list * jsonList, char * category, char * str_value); /* Add value to category */

void delete_json_list (json_list * jsonList); /* Deleting main json list */
 
void print_json_list (json_list * jsonList); /* Print json list for debugging purposes */

int skip_whitespaces(FILE *); /* Skipping whitespaces */

json_list * Parser(char *); /* Parsing function that reads json file */
