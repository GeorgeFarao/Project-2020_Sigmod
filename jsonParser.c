//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include "jsonParser.h"

json_list * new_json_list(void)
{
    json_list * list = malloc(sizeof(json_list));
    list->start = NULL;
    list->end = NULL;
    list->number_of_categories = 0;

    return list;
}

json_node * new_json_node(char * category)
{
    json_node * node = malloc(sizeof(json_node));

    node->category = malloc(strlen(category) + 1);
    strcpy(node->category, category);

    node->next = NULL;
    node->values = new_list();

    return node;
}

void insert_json_list_node (json_list * jsonList, json_node * node)
{
    if(jsonList->start == NULL)
    {
        jsonList->start = node;
        jsonList->end = node;
    }
    else
    {
        jsonList->end->next = node;
        jsonList->end = node;
    }

    jsonList->number_of_categories += 1;
}