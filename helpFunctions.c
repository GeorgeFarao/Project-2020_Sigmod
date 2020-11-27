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
void reading_directory (char * directory_name, HashTable * Table)
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
                    json_list * jsonList = Parser(second_full_path); 

                    /* Inset json record to our structures */
                    insert_Record(name, Table, jsonList);

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
