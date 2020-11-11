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

void insert_Record (char * Record , HashTable * table, json_list * jsonList)
{
    if (table==NULL)
    {
        printf("Table is NULL programm will exit");
        exit(1);
    }
    
    int hash_index = hash1(Record, table->size);
    
    if(table->Trees[ hash_index ] ==NULL)                   //If tree not exists
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     //we create tree
        RBTinsert(table->Trees[ hash_index ], new_node(Record, jsonList) );

    }
    else
        RBTinsert(table->Trees[ hash_index ], new_node(Record, jsonList));
    
    
}

void match_same_products(HashTable * table , char * spec_id1 , char * spec_id2 )
{
    
    
    int index1 = hash1(spec_id1, table->size);
    int index2 = hash1 (spec_id2,  table->size );
    
    struct node * tree_node1;
    struct node * tree_node2;


    tree_node1 = find_key_RBtree(table->Trees[index1], spec_id1);
    tree_node2 = find_key_RBtree(table->Trees[index2], spec_id2);

    if(tree_node1 ==NULL || tree_node2== NULL)
    {
        printf("Product not found\n");
        return ;
    }

    lnode * lnode_temp = tree_node2->list_same_jsons->start;
    
    if (tree_node1->list_same_jsons == tree_node2->list_same_jsons) //If already  in same list do nothing
        return ;

    tree_node1->list_same_jsons->end->next = tree_node2->list_same_jsons->start;
    tree_node1->list_same_jsons->end = tree_node2->list_same_jsons->end;

    tree_node1->list_same_jsons->size += tree_node2->list_same_jsons->size;

    
    
    while( lnode_temp!=NULL)
    {
        struct node * neighbour;

        int neighbour_hash_index = hash1(lnode_temp->json_name, table->size);
        
        neighbour = find_key_RBtree( table->Trees[ neighbour_hash_index ],  lnode_temp->json_name );
        free(neighbour->list_same_jsons);           //check here if segme
        neighbour->list_same_jsons = tree_node1->list_same_jsons;
        
        lnode_temp = lnode_temp->next;
    }
    
    
}

void print_commons(HashTable * table )
{
    for(int i=0 ;i<table->size ; i++)
    {
        if( table->Trees[i]!=NULL)
            postorder_print_commons(table->Trees[i], table->Trees[i]->root);
            
    }
}


void delete_hashtable(HashTable * table)
{
    for (int i=0; i<table->size ;i++)
    {
        if(table->Trees[i]!=NULL)
            destroyRBTree(table->Trees[i], table->Trees[i]->root);
        
        
    }
    free(table->Trees);


}
