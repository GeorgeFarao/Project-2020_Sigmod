//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#include "HashTable.h"


HashTable * newHashTable(int size)
{
    HashTable * newTable= malloc(sizeof(HashTable));
    newTable->Trees = malloc ( sizeof(struct RBTree *)  * (size)  );
    
    for(int i=0; i < size ; i++)
        newTable->Trees[i]=NULL;
    
    newTable->size=size;
    
    return newTable;
    
}




unsigned int hash1(char *str, unsigned int HASHSIZE)    // hash function by Dan Bernstein
// inspired by http://www.cs.yorku.ca/~oz/hash.html
{
    unsigned int hashval;
    
    for (hashval = 0; *(str) != '\0'; str++)
        hashval = *str + 31*hashval;
    
    return hashval % HASHSIZE;
}

void insert_Record (char * Record , HashTable * table)
{
    int hash_index = hash1(Record, table->size);
    
    if(table->Trees[ hash_index ] ==NULL)                   //If tree not exists
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     //we create tree
        RBTinsert(table->Trees[ hash_index ], new_node(Record) );

    }
    else
        RBTinsert(table->Trees[ hash_index ], new_node(Record) );
    
    
}


