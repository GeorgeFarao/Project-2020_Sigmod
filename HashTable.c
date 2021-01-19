//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"


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
void insert_Record (char * Record , HashTable * table, json_list * jsonList,int words_count)
{
    if (table==NULL)
    {
        printf("Table is NULL programm will exit");
        exit(1);
    }

    /* Find correct bucket for the Record */
    int hash_index = hash1(Record, table->size);

    struct node * temp;
    
    
    
    if(table->Trees[ hash_index ] ==NULL)                   /* If tree does not exist */
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     /* we create tree */

        temp=new_node(Record, jsonList,NO_PARAMETER,NO_PARAMETER);
        temp->bow_tf_idf_index=global_index;
        temp->number_of_words= words_count;
        RBTinsert(table->Trees[ hash_index ], temp );     /* We create and insert a node to the tree */
    }
    else            /* tree exits */
    {
        temp=new_node(Record, jsonList,NO_PARAMETER,NO_PARAMETER);
        temp->bow_tf_idf_index=global_index;
        temp->number_of_words=words_count;
        RBTinsert(table->Trees[ hash_index ], temp );
        
        
        /* We create and insert a node to the tree */
    }
}


void insert_Record_clone (char * Record , HashTable * table, json_list * jsonList,int words_count, struct node * selfnode)
{
    if (table==NULL)
    {
        printf("Table is NULL programm will exit");
        exit(1);
    }
    
    /* Find correct bucket for the Record */
    int hash_index = hash1(Record, table->size);
    
    struct node * temp;
    
    
    
    if(table->Trees[ hash_index ] ==NULL)                   /* If tree does not exist */
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     /* we create tree */
        
        temp=new_node(Record, jsonList,NO_PARAMETER,NO_PARAMETER);
        //temp->bow_tf_idf_index=global_index;
        //temp->number_of_words= words_count;
        temp->self_node = selfnode;
        RBTinsert(table->Trees[ hash_index ], temp );     /* We create and insert a node to the tree */
    }
    else            /* tree exits */
    {
        temp=new_node(Record, jsonList,NO_PARAMETER,NO_PARAMETER);
        //temp->bow_tf_idf_index=global_index;
        //temp->number_of_words=words_count;
        temp->self_node = selfnode;
        RBTinsert(table->Trees[ hash_index ], temp );
        
        
        /* We create and insert a node to the tree */
    }
}



void insert_bow (char * Record , HashTable * table, int total_files)
{
    if (table==NULL)
    {
        printf("Table is NULL programm will exit");
        exit(1);
    }
    
    /* Find correct bucket for the Record */
    int hash_index = hash1(Record, table->size);
    
    struct node * temp;
    int res;
    
    
    if(table->Trees[ hash_index ] ==NULL)                   /* If tree does not exist */
    {
        table->Trees [hash_index] = new_RBTree("char *directory_name");     /* we create tree */
        
        
        temp=new_node(Record, NULL,BOW_TF_IDF ,total_files);
        temp->bow_tf_idf_index = global_index;
        temp->word_id = global_total_words;                                 /*  Each Word has a unique word id   */
        RBTinsert_bow_tf(table->Trees[ hash_index ], temp ,total_files);     /* We create and insert a node to the tree */
        global_total_words++;

        
        
    }
    else            /* tree exits */
    {
        temp=new_node(Record, NULL,BOW_TF_IDF ,total_files);
        temp->bow_tf_idf_index=global_index;
        res = RBTinsert_bow_tf(table->Trees[ hash_index ], temp ,total_files);
        if (res ==0)
        {
            free (temp->key);
            free (temp);
        }
        else{
            temp->word_id=global_total_words;
            global_total_words++;
        }
        
        /* We create and insert a node to the tree */
    }
}



void match_different_products (HashTable * table , char * spec_id1 , char * spec_id2 )
{
    /* Find indexes of the ids in the Hashtable */
    int index1 = hash1(spec_id1, table->size);
    int index2 = hash1 (spec_id2,  table->size );

    struct node * tree_node1;
    struct node * tree_node2;

    /* Find the nodes that we need to say they are different*/
    tree_node1 = find_key_RBtree(table->Trees[index1], spec_id1);
    tree_node2 = find_key_RBtree(table->Trees[index2], spec_id2);

    if(tree_node1 ==NULL || tree_node2== NULL)
    {
        printf("Product not found\n");
        return ;
    }

    /* When we find two different nodes:  we insert the clique of first node in the list of different cliques of second node  */
    
    /* A clique is represented by its first element */
    /* Cliques change but we control it elsewhere*/
    
    
    lnode * lnode_temp = tree_node2->list_same_jsons->start;

    struct node * neighbour;
    struct node * newNode;

    int index_neig =hash1 (lnode_temp->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], lnode_temp->json_name);

    newNode = new_node(lnode_temp->json_name, NULL ,DIFFERENT_CLIQUES ,NO_PARAMETER );
    newNode->self_node=neighbour;

    int res=-1;
    res= RBTinsert(tree_node1->list_same_jsons->different_cliques, newNode);
    if(res==0)
    {
        free(newNode->key);
        free(newNode);
    }

    
    /* and then  we insert the clique of second node in the list of different cliques of first node  */

    
    lnode_temp = tree_node1->list_same_jsons->start;


    index_neig =hash1 (lnode_temp->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], lnode_temp->json_name);


    newNode = new_node(lnode_temp->json_name, NULL , DIFFERENT_CLIQUES ,NO_PARAMETER);
    newNode->self_node=neighbour;
    res=-1;
    res= RBTinsert(tree_node2->list_same_jsons->different_cliques, newNode);
    if(res==0)
    {
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

    

    
    
    /*We find all different products of first node and update them
    so they are different with the products that are same with second node */
    
    combine_tree_list(table, tree_node1->list_same_jsons->different_cliques,
                      tree_node1->list_same_jsons->different_cliques->root, tree_node2->list_same_jsons->start);

    /*We find all different products of second node and update them
     so they are different with the products that are same with first node*/
    
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
            destroy_diffRBTree(neighbour->list_same_jsons->printed_different_cliques, neighbour->list_same_jsons->printed_different_cliques->root);
            free(neighbour->list_same_jsons);
        }

        /* Make neighbour point to the new merged list */
        neighbour->list_same_jsons = tree_node1->list_same_jsons;
        lnode_temp = lnode_temp->next;
    }
}

int checkIfelementinClique(list * clique, char * element)
{
    
    lnode * lnode_temp = clique->start;
    while(lnode_temp !=NULL)
    {
        if(strcmp(lnode_temp->json_name, element)==0)
            return 1;
        
        lnode_temp = lnode_temp->next;
    }
    return 0;
}


int checkIfConflict(HashTable* table, struct node * node1, struct node * node2, int matchflag)
{
    int flag=0;

    

        lnode * lnode_temp = node1->list_same_jsons->start;
        while (lnode_temp !=NULL)
        {
            if(strcmp(lnode_temp->json_name, node2->key)==0)
                return 1;
            
            lnode_temp =lnode_temp->next;
        }
        
    
    
    
    return 0;
    
}













/* Prints all the pairs */
void print_commons(HashTable * table )
{
    for(int i=0 ;i<table->size ; i++)
    {
        if( table->Trees[i]!=NULL)
            postorder_print_commons(table->Trees[i], table->Trees[i]->root,table);

    }
}


void find_conflicts(HashTable * table,logistic_regression * model )
{
    for(int i=0 ;i<table->size ; i++)
    {
        if( table->Trees[i]!=NULL)
            postorder_findCliques_conflicts(table->Trees[i], table->Trees[i]->root, table, model);
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
    free(table);

}

void destroy_HashTable(HashTable * table)
{
    for (int i=0; i<table->size ;i++)
    {
        if(table->Trees[i]!=NULL)
            destroy_diffRBTree(table->Trees[i], table->Trees[i]->root);


    }
    free(table->Trees);

    free(table);
}


/*We find all products that are different with a node and update
 them so that they are different with the products that are same to the node */
void combine_tree_list (HashTable * table, struct RBTree * Tree1, struct node * recursion_root, lnode * start)
{
    if(recursion_root == Tree1->NIL)
        return;
    
    combine_tree_list(table, Tree1, recursion_root->left, start);
    combine_tree_list(table, Tree1, recursion_root->right, start);


    struct node * neighbour;
    int index_neig ;
    struct node * newNode;


    index_neig =hash1 (start->json_name ,  table->size );

    neighbour = find_key_RBtree(table->Trees[index_neig], start->json_name);


    newNode =  new_node(start->json_name, NULL , DIFFERENT_CLIQUES , NO_PARAMETER);
    newNode->self_node= neighbour;
    int res=-1;
    res=RBTinsert( recursion_root->self_node->list_same_jsons->different_cliques , newNode);
    if(res==0)
    {
        free(newNode->key);
        free(newNode);
    }
}


/*For a clique we save in a tree all cliques that are different with this clique. We keep one node from
 each clique but when two nodes are getting same we have saved the clique twich so we remove it */

void fix_duplicates(struct RBTree * Tree1, struct node * recursion_root , struct RBTree * Tree2 )
{
    if(recursion_root== Tree1->NIL)
        return;
    
    
    fix_duplicates(Tree1, recursion_root->left, Tree2);
    fix_duplicates(Tree1, recursion_root->right, Tree2);
    
    struct node * Newnode;
    
    /* We insert nodes to a second Tree sorted according to clique*/
    Newnode = new_node(	recursion_root->self_node->list_same_jsons->start->json_name, NULL ,DIFFERENT_CLIQUES ,NO_PARAMETER);
    Newnode->self_node = recursion_root->self_node;
    Newnode->self_node->list_same_jsons->Removed_duplicates=1;
    
    int res=-1;
    res=  RBTinsert(Tree2, Newnode);
    if(res==0){
        free(Newnode->key);
        free(Newnode);
    }
}

void remove_duplicates (HashTable * table)
{
    for (int i=0 ;i <table->size ;i++)
    {
        if (table->Trees[i]!=NULL)
            postorder_remove_duplicates(table->Trees[i] , table->Trees[i]->root);
    }
    
    
}


void print_all_different (HashTable * table)
{
    for (int i=0 ;i <table->size ;i++)
    {
        if (table->Trees[i]!=NULL)
            postorder_print_different(table->Trees[i] , table->Trees[i]->root,table);
    }
    
    
}

void create_tfidf_bow(HashTable *files, HashTable *different_words)
{
    for (int i = 0; i < files->size; i++)
    {
        if (files->Trees[i] != NULL)
        {
            postorder_initialize_bow_tfidf(files->Trees[i], files->Trees[i]->root, different_words);
        }
    }
}



HashTable * CloneTable(HashTable * table)
{
    HashTable * newTable = newHashTable(table->size);
    
    for (int i=0 ;i<table->size;i++)
        postorder_getAllRecords(table->Trees[i], table->Trees[i]->root, newTable);
    
    
    return newTable;
    
    
}




