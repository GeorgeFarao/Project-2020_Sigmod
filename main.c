//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "HashTable.h"

#define HASHTABLESIZE 128


int get_max_size_line(char *file_name){
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


//Must be executed with argv[1] = absolute path to directory W
int main(int argc, char * argv[]) {



    DIR* Fd;
    struct dirent* my_file;
    char * name = malloc(1100);
    HashTable * Table = newHashTable(HASHTABLESIZE);
    
    if (argc!=2)
    {
        printf("Wrong input\nPlzGive Directory with specks and .csv file");
        return 5;
    }
    
    if( argv[1][ strlen(argv[1]) -1] =='/'  )
        argv[1][ strlen(argv[1]) ] = '\0';

    
    
    Fd = opendir (argv[1]);
    if(Fd == NULL )
    {
        perror("Didnt open Directory");
        return 1;
    }

    //we read dataset X
    while ((my_file = readdir(Fd)) != NULL)
    {       //read directory and count subdirs

        if (strcmp(my_file->d_name, ".") != 0 && strcmp(my_file->d_name, "..") != 0)
        {
            DIR * in_directory;
            struct dirent * file_json;
            char *full_path = malloc(strlen((argv[1]))+strlen(my_file->d_name)+2);
            sprintf(full_path,"%s/%s",argv[1],my_file->d_name);
            full_path[strlen(full_path)]='\0';
            in_directory = opendir(full_path);
            
            if (in_directory == NULL)
            {
                perror("Didnt open inner directory");
                return 1;
            }
            
            while ((file_json = readdir(in_directory)) != NULL)
            {
                if (strcmp(file_json->d_name, ".") != 0 && strcmp(file_json->d_name, "..") != 0)
                {
                    sprintf(name, "%s//%s",my_file->d_name , file_json->d_name);
                    name[ strlen(name) - 5 ] = '\0';
                    char *second_full_path=malloc(strlen(full_path)+strlen(file_json->d_name)+2);
                    sprintf(second_full_path,"%s/%s",full_path,file_json->d_name);
                    json_list * jsonList = Parser(second_full_path);
                    insert_Record(name, Table,jsonList);
                    free(second_full_path);
                }
            }

            free(full_path);
            closedir(in_directory);
        }

    }


    int buffer_size = get_max_size_line(argv[2]);
    char * buffer = malloc(buffer_size);

    FILE * dataset_w = fopen(argv[2],"r");
    fgets(buffer,buffer_size,dataset_w);

    while (fgets(buffer,buffer_size,dataset_w)!=NULL)
    {   
        char * str1, * str2, * value;
        
        buffer[strlen(buffer)-1]='\0';
        str1=strtok(buffer,",");
        str2= strtok(NULL, ",");
        value=strtok(NULL, ",");
        
        if(value==NULL) 
            break;

        int real_value = atoi(value);

        if (real_value == 1)
            match_same_products(Table,str1,str2);
    }

    print_commons(Table);

    delete_hashtable(Table);
    closedir(Fd);
    fclose(dataset_w);
    free(name);
    free(buffer);
    free(Table);
}
