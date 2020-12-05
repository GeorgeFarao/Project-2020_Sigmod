//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"


/* Create new HashTable */
HashTable * newHashTable(int size)
{
    HashTable * newTable= malloc(sizeof(HashTable));
    newTable->Trees = malloc ( sizeof(struct RBTree *)  * (size)  );        //Allocate memory for table's trees

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

/* Insert record to the table */
void insert_Record (char * Record , HashTable * table, json_list * jsonList)
{
    if (table==NULL)
    {
        printf("Table is NULL programm will exit");
        exit(1);
    }

    /* Find correct bucket for the Record */
    int hash_index = hash1(Record, table->size);

    if(table->Trees[ hash_index ] ==NULL)                   /* If tree does not exist */
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     /* we create tree */
        RBTinsert(table->Trees[ hash_index ], new_node(Record, jsonList) );     /* We create and insert a node to the tree */

    }
    else            /* tree exits */
    {
        RBTinsert(table->Trees[hash_index],new_node(Record, jsonList) );  /* We create and insert a node to the tree */
    }
}



void match_different_products (HashTable * table , char * spec_id1 , char * spec_id2 )
{
    /* Find indexes of the ids in the Hashtable */
    int index1 = hash1(spec_id1, table->size);
    int index2 = hash1 (spec_id2,  table->size );

    struct node * tree_node1;
    struct node * tree_node2;

    /* Find the nodes that we need */
    tree_node1 = find_key_RBtree(table->Trees[index1], spec_id1);
    tree_node2 = find_key_RBtree(table->Trees[index2], spec_id2);

    if(tree_node1 ==NULL || tree_node2== NULL)
    {
        printf("Product not found\n");
        return ;
    }

    lnode * lnode_temp = tree_node2->list_same_jsons->start;

    struct node * neighbour;
    struct node * newNode;

    int index_neig =hash1 (lnode_temp->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], lnode_temp->json_name);

    newNode = new_node(lnode_temp->json_name, NULL);
    newNode->self_node=neighbour;

    int res=-1;
    res= RBTinsert(tree_node1->list_same_jsons->different_cliques, newNode);
    if(res==0){
        free(newNode->key);
        free(newNode);
    }

    lnode_temp = tree_node1->list_same_jsons->start;


    index_neig =hash1 (lnode_temp->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], lnode_temp->json_name);


    newNode = new_node(lnode_temp->json_name, NULL);
    newNode->self_node=neighbour;
     res=-1;
    res= RBTinsert(tree_node2->list_same_jsons->different_cliques, newNode);
    if(res==0){
        free(newNode->key);
        free(newNode);
    }

}




/* Matches same products and creates cliques */
void match_same_products(HashTable * table , char * spec_id1 , char * spec_id2 )
{

    /* Find indexes of the ids in the Hashtable */
    int index1 = hash1(spec_id1, table->size);
    int index2 = hash1 (spec_id2,  table->size );

    struct node * tree_node1;
    struct node * tree_node2;

    /* Find the nodes that we need */
    tree_node1 = find_key_RBtree(table->Trees[index1], spec_id1);
    tree_node2 = find_key_RBtree(table->Trees[index2], spec_id2);

    if(tree_node1 ==NULL || tree_node2== NULL)
    {
        printf("Product not found\n");
        return ;
    }

    /* make temp pointer point to second node's list */
    lnode * lnode_temp = tree_node2->list_same_jsons->start;

    /* If they are already  in same list (clique) do nothing */
    if (tree_node1->list_same_jsons == tree_node2->list_same_jsons)
        return ;

    combine_tree_list(table, tree_node1->list_same_jsons->different_cliques,
                      tree_node1->list_same_jsons->different_cliques->root, tree_node2->list_same_jsons->start);


    combine_tree_list(table, tree_node2->list_same_jsons->different_cliques,
                      tree_node2->list_same_jsons->different_cliques->root, tree_node1->list_same_jsons->start);


    combine_trees(tree_node2->list_same_jsons->different_cliques,
                  tree_node2->list_same_jsons->different_cliques->root,tree_node1->list_same_jsons->different_cliques);
   // destroy_diffRBTree(tree_node2->list_same_jsons->different_cliques, tree_node2->list_same_jsons->different_cliques->root);
    /* Make first node's end point to the start of second node's list  and it's end to the second's end*/
    tree_node1->list_same_jsons->end->next = tree_node2->list_same_jsons->start;
    tree_node1->list_same_jsons->end = tree_node2->list_same_jsons->end;
    tree_node1->list_same_jsons->size += tree_node2->list_same_jsons->size;



    /* We traverse second node's list and make each node in the list point to first's list */
    while( lnode_temp!=NULL)
    {
        struct node * neighbour;
        /* Find Every Node of the clique */
        int neighbour_hash_index = hash1(lnode_temp->json_name, table->size);
        neighbour = find_key_RBtree( table->Trees[ neighbour_hash_index ],  lnode_temp->json_name );

        /* Only one node from the old list must delete it because they all point to same list item that contains
	and end information of the list   */

        if(strcmp(neighbour->list_same_jsons->end->json_name,neighbour->key)==0){
            destroy_diffRBTree(neighbour->list_same_jsons->different_cliques, neighbour->list_same_jsons->different_cliques->root);
            free(neighbour->list_same_jsons);
        }

        /* Make neighbour point to the new merged list */
        neighbour->list_same_jsons = tree_node1->list_same_jsons;
        lnode_temp = lnode_temp->next;
    }
}


/* Prints all the pairs */
void print_commons(HashTable * table )
{
    for(int i=0 ;i<table->size ; i++)
    {
        if( table->Trees[i]!=NULL)
            postorder_print_commons(table->Trees[i], table->Trees[i]->root);

    }
}

/* Deletes HashTable */
void delete_hashtable(HashTable * table)
{
    for (int i=0; i<table->size ;i++)
    {
        if(table->Trees[i]!=NULL)
            destroyRBTree(table->Trees[i], table->Trees[i]->root);


    }
    free(table->Trees);


}




void combine_tree_list (HashTable * table, struct RBTree * Tree1, struct node * recursion_root, lnode * start)
{

    if(recursion_root == Tree1->NIL) {

        return;
    }
    combine_tree_list(table, Tree1, recursion_root->left, start);
    combine_tree_list(table, Tree1, recursion_root->right, start);


    struct node * neighbour;
    int index_neig ;
    struct node * newNode;


    index_neig =hash1 (start->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], start->json_name);


    newNode =  new_node(start->json_name, NULL);
    newNode->self_node= neighbour;
    int res=-1;
    res=RBTinsert( recursion_root->self_node->list_same_jsons->different_cliques , newNode);
    if(res==0){
        free(newNode->key);
        free(newNode);
    }


}
