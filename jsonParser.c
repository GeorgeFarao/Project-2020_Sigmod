/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#include "jsonParser.h"


/* Creating main json list with product info */
json_list * new_json_list(void)
{
    json_list * list = malloc(sizeof(json_list));
    list->start = NULL;
    list->end = NULL;
    list->number_of_categories = 0;

    return list;
}


/* New json node - New category to product */
json_node * new_json_node(char * category)
{
    json_node * node = malloc(sizeof(json_node));

    node->category = malloc(strlen(category) + 1);
    strcpy(node->category, category);

    node->next = NULL;
    node->values = new_list();

    return node;
}


/* Insert json node - category to json list */
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


/* Add value to category */
void add_category_value (json_list * jsonList, char * category, char * str_value)
{
    json_node * temp_node = jsonList->start;
    lnode * json_node_value = new_lnode(str_value);

    if (temp_node == NULL) /* No json_nodes in json_list case */
    {
        temp_node = new_json_node(category);
        insert_lnode(temp_node->values,  json_node_value);
        jsonList->start = temp_node;
        jsonList->end = temp_node;
        jsonList->number_of_categories += 1;  
    }
    else
    {
        while (temp_node != NULL) /* Check if category exists */
        {
            if (strcmp(temp_node->category, category) == 0) /* Found category so insert value at json_node */
            {
                insert_lnode(temp_node->values,  json_node_value);
                return;
            }

            temp_node = temp_node->next;
        }

        /* No category found so create category and add value */

        temp_node = new_json_node(category); /* Initialize json_node - category */
        insert_lnode(temp_node->values,  json_node_value); /* Insert value at json_node - category */
        insert_json_list_node (jsonList, temp_node); /* Insert json_node - category with values - at json_list */
    }
}


/* Deleting main json list */
void delete_json_list (json_list * jsonList)
{
    if (jsonList->start == NULL)
    {
        printf("List is empty (delete_json_list)\n");
        return;
    }
    else
    {
        json_node *prev = jsonList->start;
        
        while(jsonList->start != NULL)
        {
            jsonList->start = jsonList->start->next;
            delete_list(prev->values);
            free(prev->values);
            free(prev->category);
            free(prev);
            prev = jsonList->start;
        }
    }
    
    jsonList->start = jsonList->end = NULL;    
}


/* Print json list for debugging purposes */
void print_json_list (json_list * jsonList)
{
    json_node * temp_node = jsonList->start;

    while (temp_node != NULL)
    {
        lnode * temp_lnode = temp_node->values->start;
        printf("|||  %s  |||\n", temp_node->category);
        
        while (temp_lnode != NULL)
        {
            printf("%s ", temp_lnode->json_name);
            temp_lnode = temp_lnode->next;
        }
        printf("\n");

        temp_node = temp_node->next;
    } 
}


/* Skipping whitespaces */
int skip_whitespaces(FILE * file){
    int ch=fgetc(file);
    while (ch == '\n' || ch == ' ' || ch == '\t')
        ch=fgetc(file);
    return ch;
}


/* Parsing function that reads json file */
json_list * Parser(char * file)
{
    char * buffer= malloc(100000);
    /* First we open the json file */
    FILE * fptr = fopen(file,"r");
    json_list * json_list= new_json_list();
    /* Check if fopen was successful */
    if(fptr==NULL)
    {
        printf("File not found\n");
        return NULL;
    }

    char ch;
    char prev_ch;
    /* Skip whitespaces and find first character */
    ch=(char)skip_whitespaces(fptr);
    int count=0;
    char *category=malloc(10000);
    if(ch!='{')
        return NULL;

    /* Read each line from the json file */
    while (1)
    {
        prev_ch=ch;
        count=0;
        ch = (char)skip_whitespaces(fptr);      /* Get to the first non whitespace character of each line */
        prev_ch=ch;

        /* First read the category */
        while (1){
            prev_ch=ch;
            ch = (char) fgetc(fptr);
            category[count]=(char)ch;      /* We read the whole string and store it in the buffer */
            count++;
            int flag=0;

            if (ch=='"'){                   /* When we find " character we have read the whole word so we break */
                category[count-1]='\0';
                count=0;
                break;
            }

        }

        ch=(char)skip_whitespaces(fptr);        //skip :
        ch=(char)skip_whitespaces(fptr);        //finds starting character of value

        /* There are two cases */
        /* either we have a simple value or we have a list of values */

        if(ch=='"')         /* Simple values case */
        {
            while (1)
            {
                prev_ch = ch;
                ch = (char) fgetc(fptr);
                buffer[count] = (char) ch;          /* We store the value in the buffer */
                count++;
                if(ch=='}' && prev_ch!='\\') {          /* When we find } without \ before we ahve reached the end of the file */
                    buffer[count - 3] = '\0';
                    break;
                }

                /* When we find , which is not ing the beginning of the word */
                /* and prev is " value ends so we break */
                if (ch == ','  && count>1 ) {
                    if (prev_ch == '"' && buffer[count-3]!='\\' ) {
                        break;
                    }
                }
            }
            buffer[count-2]='\0';
            add_category_value(json_list,category,buffer);      /* Add value into the list */
        }

        if(ch=='[')     /* List of values */
        {
            while (1)
            {
                prev_ch = ch;
                ch = (char) skip_whitespaces(fptr);     /* Find each value of the list*/

                while (1)       /* Read the value */
                {
                    prev_ch = ch;
                    ch = (char) fgetc(fptr);
                    buffer[count] = (char) ch;      /* Store the value into the buffer */
                    count++;

                    /* " indicate that we have reached the end of the value */
                    if (ch == '"' && prev_ch!='\\') {
                        buffer[count-1]='\0';
                        add_category_value(json_list,category,buffer);
                        count=0;
                        break;
                    }
                }
                /* If next character is , there are still values in the list so we continue reading */
                /* If next character is ] we have reached the end of the list so we break */
                ch=skip_whitespaces(fptr);
                if (ch==',')
                    continue;
                if(ch==']'){
                    ch=skip_whitespaces(fptr);
                    break;
                }
            }
        }

        if (ch=='}'){
            break;
        }
    }

    fclose(fptr);
    free(category);
    free(buffer);
    return json_list;
}

