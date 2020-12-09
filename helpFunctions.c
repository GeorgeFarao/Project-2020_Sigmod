#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>





#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"


/* Get maximum size of a line in file */ 
int get_max_size_line(char *file_name)
{
    FILE * fp = fopen (file_name, "r");
    char c;
    int max=0;
    int num_of_chars=0;
    
    while (1)
    {
        c = fgetc(fp);
        if( feof(fp) )
            break ;
        if (c!='\n')
            num_of_chars++;
        else
        {
            if(num_of_chars>max)
                max=num_of_chars;
            num_of_chars=0;
        }
    }

    fclose(fp);
    return max+2;
}

/* Reading file and initializing structs */

void reading_directory (char * directory_name, HashTable * Table , HashTable * stopwords , HashTable * bow_tf , int total_files)
{
    DIR* Fd;
    struct dirent* directory_with_json_files;
    char * name = malloc(1100);

    if (directory_name[strlen(directory_name) - 1] =='/')
        directory_name[strlen(directory_name) - 1] = '\0';

    Fd = opendir (directory_name);
    if(Fd == NULL )
    {
        perror("Didnt open Directory");
        exit(1);
    }

    /* Reading directory with json files */
    while ((directory_with_json_files = readdir(Fd)) != NULL)
    {       
        /* Reading directory and count subdirs */
        if (strcmp(directory_with_json_files->d_name, ".") != 0 && strcmp(directory_with_json_files->d_name, "..") != 0)
        {
            DIR * in_directory; /* in directory with json files */
            struct dirent * file_json; /* For every json file */

        
            
            /* Path includes directory and subdirectory */
            char *full_path = malloc(strlen((directory_name))+strlen(directory_with_json_files->d_name)+2); 
            sprintf(full_path,"%s/%s",directory_name,directory_with_json_files->d_name); 
            full_path[strlen(full_path)]='\0'; 
            
            in_directory = opendir(full_path);
            
            if (in_directory == NULL)
            {
                perror("Didnt open inner directory");
                exit(1);
            }
            
            /* Reading all json files of subdirectory */
            while ((file_json = readdir(in_directory)) != NULL)
            {
                if (strcmp(file_json->d_name, ".") != 0 && strcmp(file_json->d_name, "..") != 0)
                {
                    /* Name of each json file record */
                    sprintf(name, "%s//%s",directory_with_json_files->d_name , file_json->d_name); 
                    name[ strlen(name) - 5 ] = '\0';

                    /* Full path from directory to json file */
                    char *second_full_path = malloc(strlen(full_path) + strlen(file_json->d_name)+2); 
                    sprintf(second_full_path,"%s/%s",full_path,file_json->d_name);
                    
                    /* Extracting info's from jason files and initialzing jsonList */
                    /* jsonList includes categories and values of categories for current json file */
                    json_list * jsonList = Parser(second_full_path, stopwords, bow_tf, total_files);

                    /* Inset json record to our structures */
                    insert_Record(name, Table, jsonList);
                    global_index++;
                    free(second_full_path);
                }
            }

            free(full_path);
            closedir(in_directory);
        }

    }

    free(name);
    closedir(Fd);
}


/* Reading csv file and matching same products */ 
void reading_csv_file (char * file_name, HashTable * Table)
{
    int buffer_size = get_max_size_line(file_name);
    char * buffer = malloc(buffer_size);

    FILE * dataset_w = fopen(file_name,"r"); /* open csv file */
    fgets(buffer,buffer_size,dataset_w); /* ignore first line */

    while (fgets(buffer,buffer_size,dataset_w)!=NULL) /* Main loop of reading */
    {   
        char * str1, * str2, * value;
        
        buffer[strlen(buffer)-1]='\0';
        str1 = strtok(buffer,","); /* first json_file id */
        str2 = strtok(NULL, ","); /* second json file_id */
        value = strtok(NULL, ","); /* matching flag */
        
        if (value == NULL) 
            break;

        int matching_flag = atoi(value);

        /* Check if json files match */
        if (matching_flag == 1) 
            match_same_products(Table,str1,str2);
        else
            match_different_products(Table, str1, str2);
        
    }

    print_commons(Table); /* Printing matching json files */   

    fclose(dataset_w);
    free(buffer); 
}

HashTable * create_stopwords_Hash(char *filename)
{
    char  c=' ';
    int count=0;
    HashTable * Table= newHashTable(10);
    char * buffer = malloc(100);
    FILE * file = fopen(filename,"r");
    while(c!=EOF)
    {
        c=fgetc(file);
        if(c==',')
        {
           buffer[count]='\0';
           insert_Record(buffer,Table,NULL);
           count=0;
        }
        else if (c==EOF)
        {
            buffer[count]='\0';
            insert_Record(buffer,Table,NULL);
            break;
        }
        else{
           buffer[count]=c;
            count++;
        }

    }
    free(buffer);
    fclose(file);
    return Table;

}

int count_number_of_files(char * directory_name)
{
    int counter = 0;
    
    DIR* Fd;
    struct dirent* directory_with_json_files;
    char * name = malloc(1100);
    
    if (directory_name[strlen(directory_name) - 1] =='/')
        directory_name[strlen(directory_name) - 1] = '\0';
    
    Fd = opendir (directory_name);
    if(Fd == NULL )
    {
        perror("Didnt open Directory");
        exit(1);
    }
    
    /* Reading directory with json files */
    while ((directory_with_json_files = readdir(Fd)) != NULL)
    {
        /* Reading directory and count subdirs */
        if (strcmp(directory_with_json_files->d_name, ".") != 0 && strcmp(directory_with_json_files->d_name, "..") != 0)
        {
            DIR * in_directory; /* in directory with json files */
            struct dirent * file_json; /* For every json file */
            
            
            
            /* Path includes directory and subdirectory */
            char *full_path = malloc(strlen((directory_name))+strlen(directory_with_json_files->d_name)+2);
            sprintf(full_path,"%s/%s",directory_name,directory_with_json_files->d_name);
            full_path[strlen(full_path)]='\0';
            
            in_directory = opendir(full_path);
            
            if (in_directory == NULL)
            {
                perror("Didnt open inner directory");
                exit(1);
            }
            
            /* Reading all json files of subdirectory */
            while ((file_json = readdir(in_directory)) != NULL)
            {
                if (strcmp(file_json->d_name, ".") != 0 && strcmp(file_json->d_name, "..") != 0)
                    counter++;

            }
            
            free(full_path);
            closedir(in_directory);
        }
        
    }
    
    free(name);
    closedir(Fd);
    return counter;
    
    
}




void process_string(char * string,json_list * list,char * category, HashTable * stopWords, HashTable * bow_tf_idf , int totalfiles  )
{
    int size= strlen(string);
    char * new_buf= malloc(size+1);
    int c=0;
    int new_count=0;
    if(strcmp(category,"url")==0)
    {
        add_category_value(list,category,new_buf);
        insert_bow(new_buf, bow_tf_idf, totalfiles);
        return;

    }
    while (string[c] !='\0')
    {
        if(string[c]>='A' && string[c]<='Z')
        {
            new_buf[new_count] = string[c] + 32;
            new_count++;
        }
        else if ( string[c]=='-' && c!=0 && string[c-1]!=' ')
        {
            new_buf[new_count]=string[c];
            new_count++;
        }
        else if (string[c]==',' && c!=0 && c!=size && (string[c-1]>='0' && string[c+1]<='9'))
        {
            new_buf[new_count]='.';
            new_count++;
        }
        else if ((string[c]>='a' && string[c]<='z') || (string[c]>='0' && string[c]<='9') || string[c]=='.'
                 || string[c]==':' || string[c]=='\\' || string[c]=='_' || string[c]=='=' )
        {
            new_buf[new_count]=string[c];
            new_count++;
        }
        else if( string[c]==' ' || string[c]=='|')
        {
            

            new_buf[new_count]='\0';
            int index= hash1(new_buf,stopWords->size);
            struct node * tree_node;
            tree_node = find_key_RBtree(stopWords->Trees[index], new_buf);
            if (tree_node==NULL)
            {
                add_category_value(list,category,new_buf);
                insert_bow(new_buf, bow_tf_idf, totalfiles);
            }
            new_count=0;
             
        }
        c++;
    }

}
