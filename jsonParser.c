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

int skip_whitespaces(FILE * file){
    int ch=fgetc(file);
    while (ch == '\n' || ch == ' ' || ch == '\t')
        ch=fgetc(file);
    return ch;
}



json_list * Parser(char * file)
{
    char * buffer= malloc(1000000);
    FILE * fptr = fopen(file,"r");
    json_list * json_list= new_json_list();
    if(fptr==NULL)
    {
        printf("File not found\n");
        return NULL;
    }

    char ch;
    char prev_ch;
    ch=(char)skip_whitespaces(fptr);
    int count=0;
    char *category=malloc(10000000);
    if(ch!='{')
        return NULL;
    while (1)       //lines
    {
        prev_ch=ch;
        count=0;
        ch = (char)skip_whitespaces(fptr);      //ch=="
        //buffer[0]=(char)ch;
        //count++;

        prev_ch=ch;
        while (1){  //read lines
            prev_ch=ch;
            ch = (char) fgetc(fptr);
            category[count]=(char)ch;
            count++;
            int flag=0;
            /*if(ch=='}'){
                buffer[count-2]='\0';
                break;
            }*/
            if (ch=='"'){
                category[count-1]='\0';
                count=0;
                break;
            }

            /*if(prev_ch==' ' && ch=='['){
                while (1) {
                    prev_ch = ch;
                    ch = (char) fgetc(fptr);
                    buffer[count] = (char) ch;
                    count++;
                    if (ch == '}') {
                        buffer[count - 2] = '\0';
                        break;
                    }
                    if (ch == ',' && prev_ch == ']') {
                        flag=1;
                        break;
                    }
                }
            }*/
           /* if(flag==0)
            {
                if (ch == ',') {
                    if (prev_ch == ']' || prev_ch == '"' || prev_ch == '\n' || prev_ch == ' ' || prev_ch == '\t') {
                        break;
                    }
                }
            }
            else
                break;*/
        }



        ch=(char)skip_whitespaces(fptr);        //skip :
        ch=(char)skip_whitespaces(fptr);        //finds starting character of value

        //two cases

        if(ch=='"')
        {
            while (1)
            {
                prev_ch = ch;
                ch = (char) fgetc(fptr);
                buffer[count] = (char) ch;
                count++;
                if(ch=='}' && prev_ch!='\\') {        //end of file
                    buffer[count - 3] = '\0';
                    break;
                }


                if (ch == ',' /*!!!!!!*/ && count>1 /*!!!!!!*/) {
                    if (prev_ch == '"' && buffer[count-3]!='\\' ) {
                        break;
                    }
                }
            }
            buffer[count-2]='\0';
            //printf("%s\n",buffer);
            add_category_value(json_list,category,buffer);
        }

        if(ch=='[')
        {
            while (1)
            {
                prev_ch = ch;
                ch = (char) skip_whitespaces(fptr);     //find "
                while (1)
                {
                    prev_ch = ch;
                    ch = (char) fgetc(fptr);
                    buffer[count] = (char) ch;
                    count++;

                    if (ch == '"' && prev_ch!='\\') {
                        buffer[count-1]='\0';
                        //printf("%s\n",buffer);
                        add_category_value(json_list,category,buffer);
                        count=0;
                        break;
                    }
                }

                ch=skip_whitespaces(fptr);
                if (ch==',')
                    continue;
                if(ch==']'){
                    ch=skip_whitespaces(fptr);
                    break;
                }
                //if ()


            }

        }





        //buffer[count]='\0';
        //printf("New line: %s\n",buffer);

        if (ch=='}'){

            break;
        }
        //break;
    }

    fclose(fptr);
    free(buffer);
    return json_list;
}

