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

