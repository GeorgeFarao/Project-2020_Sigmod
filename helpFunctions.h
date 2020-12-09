

int get_max_size_line (char *file_name); /* Get maximum size of a line in file */ 

void reading_directory (char * directory_name, HashTable * Table , HashTable * stopwords , HashTable * bow_tf, int total_files);

void reading_csv_file (char * file_name, HashTable * Table); /* Reading csv file and matching same products */

int count_number_of_files(char * directory_name);
HashTable * create_stopwords_Hash(char *filename);

void process_string(char * string,json_list * list,char * category, HashTable * stopWords, HashTable * bow_tf_idf , int totalfiles  );
