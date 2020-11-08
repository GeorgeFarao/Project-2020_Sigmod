//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include <stdio.h>

#include "HashTable.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

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
    char *token;
    HashTable * Table = newHashTable(HASHTABLESIZE);
    printf("%s\n",argv[1]);
    Fd = opendir (argv[1]);
    if(Fd == NULL )
    {
        perror("Didnt open Directory");
        return 1;
    }

    //we read dataset X
    while (( my_file = readdir(Fd)) != NULL)
    {       //read directory and count subdirs

        if (strcmp(my_file->d_name, ".") != 0 && strcmp(my_file->d_name, "..") != 0)
        {
            DIR * in_directory;
            struct dirent * file_json;
            char *full_path=malloc(strlen((argv[1]))+strlen(my_file->d_name)+2);
            sprintf(full_path,"%s/%s",argv[1],my_file->d_name);
            full_path[strlen(full_path)]='\0';
            in_directory = opendir(full_path);
            if(in_directory == NULL)
            {
                perror("Didnt open inner directory");
                return 1;

            }
            while (( file_json = readdir(in_directory)) != NULL)
            {
                if (strcmp(file_json->d_name, ".") != 0 && strcmp(file_json->d_name, "..") != 0)
                {
                    sprintf(name, "%s//%s",my_file->d_name , file_json->d_name);
                    name[ strlen(name) - 5 ] = '\0';
                    insert_Record(name, Table);

                }
            }
        }

    }


    int buffer_size= get_max_size_line(argv[2]);
    char * buffer= malloc(buffer_size);

    FILE * dataset_w = fopen(argv[2],"r");
    fgets(buffer,buffer_size,dataset_w);

    while (fgets(buffer,buffer_size,dataset_w)!=NULL)
    {   char * str1, * str2, * value;
        buffer[strlen(buffer)-1]='\0';
        str1=strtok(buffer,",");
        //str1=token;
        str2= strtok(NULL, ",");
        //str2=token;
        value=strtok(NULL, ",");
        if(value==NULL) {
            break;
        }
        int real_value= atoi(value);
        if(real_value ==1)
            match_same_products(Table,str1,str2);

        
    }

    print_commons(Table);






}
