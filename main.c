//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include <stdio.h>

#include "list.h"
#include "HashTable.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define HASHTABLESIZE 128

int main(int argc, const char * argv[]) {
    
    
    
    DIR* Fd;
    struct dirent* my_file;
    char * name = malloc(1100);
    HashTable * Table = newHashTable(HASHTABLESIZE);
    printf("%s\n",argv[1]);
    Fd = opendir (argv[1]);
    if(Fd == NULL )
    {
        perror("Didnt open Directory");
        return 1;
    }
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
}

