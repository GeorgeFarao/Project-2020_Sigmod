//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#ifndef HashTable_h
#define HashTable_h



typedef struct HashTable{

    struct RBTree ** Trees;
    int size;
    
    
}HashTable;


HashTable * newHashTable(int size);
unsigned int hash1(char *str, unsigned int HASHSIZE);    // hash function by Dan Bernstein
void insert_Record (char *Record , HashTable *table, json_list *, int words_count);
void match_different_products (HashTable * table , char * spec_id1 , char * spec_id2 );
void match_same_products(HashTable * table , char * spec_id1 , char * spec_id2 );
void print_commons(HashTable * table );
void delete_hashtable(HashTable * table);
void delete_hashtable_cloned(HashTable * table);
void destroy_HashTable(HashTable * table);
void combine_tree_list (HashTable * table, struct RBTree * Tree1, struct node * recursion_root, lnode * start);
void fix_duplicates(struct RBTree * Tree1, struct node * recursion_root , struct RBTree * Tree2 );

void remove_duplicates (HashTable * table);
void print_all_different (HashTable * table);
void insert_bow (char * Record , HashTable * table , int total_files);

json_list * Parser(char * file , HashTable * stopwords , HashTable * bow_tf , int totalfiles, int *words_count); /* Parsing function that reads json file */
void initialize_bow_tf_idf(struct node * file, HashTable *diffWords);
void postorder_initialize_bow_tfidf(struct RBTree * Tree, struct node *root,HashTable *diffWords);
void create_tfidf_bow(HashTable *files, HashTable *different_words);
void print_list(list * mylist , HashTable * files);
void print_two_lists(list * mylist1 ,list * mylist2, HashTable * files);
void print_different(list *clique, struct RBTree *Tree_different_cliques, struct node *recursion_root, HashTable * files);
void postorder_print_different(struct RBTree *Tree, struct node *node, HashTable * files);
void postorder_print_commons(struct RBTree *T, struct node *node, HashTable * files);
void postorder_getAllRecords(struct RBTree * Tree, struct node * root, HashTable * newTable);

HashTable * CloneTable(HashTable * table);

int checkIfelementinClique(list * clique, char * element);
void insert_Record_clone (char * Record , HashTable * table, json_list * jsonList,int words_count, struct node * selfnode);

void other_insert_Record (char * Record , HashTable * table, json_list * jsonList,int words_count);

#endif /* HashTable_h */
