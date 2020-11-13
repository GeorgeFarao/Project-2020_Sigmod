/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#include "helpFunctions.h"

#define HASHTABLESIZE 128


/* Must be executed with: */
/* argv[1] = absolute path to directory with specs */
/* argv[2] = .csv file */

int main(int argc, char * argv[]) {


    HashTable * Table = newHashTable(HASHTABLESIZE);
    
    if (argc != 3)
    {
        printf("Wrong input\nPease give Directory with specs and .csv file");
        return 5;
    }

    /* Reading input files and initializing our structs */
    reading_directory (argv[1], Table);
    reading_csv_file (argv[2], Table);

    /* Free allocated memory */
    delete_hashtable(Table);    
    free(Table);
}

