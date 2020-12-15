/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"

#define HASHTABLESIZE 128

/* Must be executed with: */
/* ./find_commons -d path_to_directory -f file_with_matching_products -h hashtable_size */

int main(int argc, char *argv[])
{

    HashTable *Table;
    HashTable *stopwords;
    HashTable *bow_tfidf;
    data = new_list_data();
    test = new_list_data();
    validation = new_list_data();

    char *directory_name = NULL;
    char *file_name = NULL;
    char *hashtable_size = NULL;
    char *stopwords_file_name = NULL;
    if (argc != 9)
    {
        printf("Wrong input\nPease give Directory with specs and .csv file and hash table size\n");
        return 5;
    }

    /* Reading parameters */
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0) /* path to directory parameter */
            directory_name = argv[i + 1];

        else if (strcmp(argv[i], "-f") == 0) /* .csv file parameter */
            file_name = argv[i + 1];

        else if (strcmp(argv[i], "-h") == 0) /* hashtable size parameter */
            hashtable_size = argv[i + 1];
        else if (strcmp(argv[i], "-sw") == 0) /* stopwords file */
            stopwords_file_name = argv[i + 1];
    }

    if (directory_name == NULL || file_name == NULL || hashtable_size == NULL)
    {
        printf("Error in parameters given !\n");
        return 0;
    }
    int total_files = count_number_of_files(directory_name);

    Table = newHashTable(atoi(hashtable_size));
    bow_tfidf = newHashTable(atoi(hashtable_size) * 1500);
    stopwords = create_stopwords_Hash(stopwords_file_name);

    /* Reading input files and initializing our structs */
    reading_directory(directory_name, Table, stopwords, bow_tfidf, total_files);
    reading_csv_file(file_name, Table);
    remove_duplicates(Table);
    print_all_different(Table);
    create_tfidf_bow(Table, bow_tfidf);
    printf("%d %d %d\n",data->size,test->size,validation->size);
    logistic_regression * model;
    model = new_model(global_total_words*2, 0, 0.00000001,5);
    train(Table,model);


    //train(Table,model);
    //train(Table,model);
    /* Free allocated memory */
    delete_hashtable(Table);
    // na ftiaksoume delete gia to stopwords
    free(Table);
}
