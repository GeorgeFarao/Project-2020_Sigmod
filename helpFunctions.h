#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "HashTable.h"

int get_max_size_line (char *file_name); /* Get maximum size of a line in file */ 

void reading_directory (char * directory_name, HashTable * Table); /* Reading file and initializing structs */
 
void reading_csv_file (char * file_name, HashTable * Table); /* Reading csv file and matching same products */

HashTable * create_stopwords_Hash(char *filename);