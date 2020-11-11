//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include "list.h"

typedef struct json_node {
    char * category;
    list * values;
    struct json_node * next;
} json_node;

typedef struct json_list {
    json_node * start;
    json_node * end;
    int number_of_categories;
} json_list;

///////////////////// FUNCTIONS /////////////////////

json_list * new_json_list (void);

json_node * new_json_node (char * category);

void add_category_value (json_list * jsonList, char * category, char * str_value);

void insert_json_list_node (json_list * jsonList, json_node * node);

void delete_json_list (json_list * jsonList);

void print_json_list (json_list * jsonList);

json_list * Parser(char *);

int skip_whitespaces(FILE *);
