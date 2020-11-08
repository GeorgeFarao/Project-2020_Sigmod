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



void add_category_value (json_list * jsonList, char * category, char * str_value)
{
    json_node * temp_node = jsonList->start;
    lnode * json_node_value = new_lnode(str_value);

    if (temp_node == NULL) // No json_nodes in json_list case
    {
        temp_node = new_json_node(category);
        insert_lnode(temp_node->values,  json_node_value);
        jsonList->start = temp_node;
        jsonList->end = temp_node;
        jsonList->number_of_categories += 1;  
    }
    else
    {
        while (temp_node != NULL)
        {
            if (strcmp(temp_node->category, category) == 0) // Found category so insert value at json_node
            {
                insert_lnode(temp_node->values,  json_node_value);
                return;
            }

            temp_node = temp_node->next;
        }

        // No category found so create category and add value

        temp_node = new_json_node(category); // Initialize json_node
        insert_lnode(temp_node->values,  json_node_value); // Insert value at json_node
        insert_json_list_node (jsonList, temp_node); // Insert json_node at json_list
    }
}

