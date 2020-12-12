//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#ifndef HashTable_h
#define HashTable_h

typedef struct HashTable
{

    struct RBTree **Trees;
    int size;
    int total_words;

} HashTable;

HashTable *newHashTable(int size);
unsigned int hash1(char *str, unsigned int HASHSIZE); // hash function by Dan Bernstein
void insert_Record(char *Record, HashTable *table, json_list *, int words_count);
void match_different_products(HashTable *table, char *spec_id1, char *spec_id2);
void match_same_products(HashTable *table, char *spec_id1, char *spec_id2);
void print_commons(HashTable *table);
void delete_hashtable(HashTable *table);
void combine_tree_list(HashTable *table, struct RBTree *Tree1, struct node *recursion_root, lnode *start);
void fix_duplicates(struct RBTree *Tree1, struct node *recursion_root, struct RBTree *Tree2);

void remove_duplicates(HashTable *table);
void print_all_different(HashTable *table);
void insert_bow(char *Record, HashTable *table, int total_files);

json_list *Parser(char *file, HashTable *stopwords, HashTable *bow_tf, int totalfiles, int *words_count); /* Parsing function that reads json file */

void postorder_initialize_bow_tfidf_row(struct RBTree *Tree, struct node *node, HashTable *different_words);
void postorder_help_to_initialize_bow_tfidf_row(struct RBTree *Tree, struct node *recursion_root,
                                                struct node *file, HashTable *different_words);

void create_tfidf_bow(HashTable *files, HashTable *different_words);
void traversing_words_for_tfidf_bow(HashTable *different_words, struct node *file);
#endif /* HashTable_h */
