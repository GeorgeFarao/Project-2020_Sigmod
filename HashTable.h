//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#ifndef HashTable_h
#define HashTable_h



typedef struct HashTable{

    struct RBTree ** Trees;
    int size;
    
    
}HashTable;


HashTable * newHashTable(int size);
unsigned int hash1(char *str, unsigned int HASHSIZE);    // hash function by Dan Bernstein
void insert_Record (char *Record , HashTable *table, json_list *);
void match_different_products (HashTable * table , char * spec_id1 , char * spec_id2 );
void match_same_products(HashTable * table , char * spec_id1 , char * spec_id2 );
void print_commons(HashTable * table );
void delete_hashtable(HashTable * table);
void combine_tree_list (HashTable * table, struct RBTree * Tree1, struct node * recursion_root, lnode * start);
void fix_duplicates(struct RBTree * Tree1, struct node * recursion_root , struct RBTree * Tree2 );

void remove_duplicates (HashTable * table);
void print_all_different (HashTable * table);

#endif /* HashTable_h */

