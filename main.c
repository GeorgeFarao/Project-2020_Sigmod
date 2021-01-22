/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"

#define HASHTABLESIZE 128

/* Must be executed with: */
/* ./find_commons -d path_to_directory -f file_with_matching_products -h hashtable_size */

int main(int argc, char *argv[])
{

    //srand(time(NULL));
    HashTable *Table;
    HashTable *stopwords;
    HashTable *bow_tfidf;
    data = new_list_data();
    test = new_list_data();
    validation = new_list_data();
    validation_to_train = new_list_data();
    allfiles =new_list();
    

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

    if (directory_name == NULL || file_name == NULL || hashtable_size == NULL || stopwords_file_name == NULL)
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
    print_commons(Table);
    create_tfidf_bow(Table, bow_tfidf);

    printf("data size: %d, validation size: %d, test size: %d\n", data->size, validation->size,test->size);

    //create_validation_list(allfiles);
    
    /* trainning our model */
    logistic_regression *model;
    model = new_model(global_total_words * 2, 0, 0.00000001, 5);

    scheduler = initialize_scheduler(model);
    
    
    for (int i=0 ; i<model->epoch;i++)
        CreateJobs(1);

    for (int i=0 ;i<NUMBER_OF_THREADS;i++)
        pthread_create(&scheduler->threadIds[i], NULL, Writer, (void * ) model );
    Reader(model, 0.01,Table);
    printf("End of testing \n");







    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
         pthread_join(*(scheduler->threadIds + i), NULL);

    /* Free allocated memory */
    delete_hashtable(Table);
    destroy_HashTable(stopwords);
    destroy_HashTable(bow_tfidf);
    delete_dataList(data);
    delete_dataList(test);
    delete_dataList(validation);
    destroy_model(model);
}
