/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "dataList.h"
#include "logistic_regression.h"
#include "thread.h"


int global_index;
int global_total_words;

/* Creating new RedBlackTree */
struct RBTree *new_RBTree(char *directory_name)
{
    struct RBTree *Tree = malloc(sizeof(struct RBTree));
    struct node *nil_node = malloc(sizeof(struct node));

    Tree->directory_name = malloc(strlen(directory_name) + 1);
    strcpy(Tree->directory_name, directory_name);

    nil_node->left = NULL;
    nil_node->right = NULL;
    nil_node->parent = NULL;

    nil_node->color = 'B';

    Tree->NIL = nil_node;
    Tree->root = Tree->NIL;

    return Tree;
}

/* Creating new tree node */
struct node *new_node(char *json_id, json_list *jsonList, int flag, int total_files)
{
    struct node *node = malloc(sizeof(struct node));

    node->key = malloc(strlen(json_id) + 1);
    strcpy(node->key, json_id);

    if (jsonList != NULL)
    {
        node->list_same_jsons = new_list(); /* List with matching json files */

        /* Initialize list with itself */
        lnode *listnode = new_lnode(json_id);
        insert_lnode(node->list_same_jsons, listnode);
        node->list_same_jsons->different_cliques = new_RBTree("Tree_For_Different_CLiques");
        node->list_same_jsons->printed_different_cliques = new_RBTree("Printed_Different_cliques");
        node->bow = NULL;
        node->tf_idf = NULL;
    }
    else if (flag == DIFFERENT_CLIQUES)
    {
        node->list_same_jsons = NULL;
        node->tf_idf = NULL;
        node->bow = NULL;
    }
    else if (flag == BOW_TF_IDF)
    {
        node->list_same_jsons = NULL;
    }
    else if (jsonList==NULL)
    {
        node->list_same_jsons=NULL;
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->json_info = jsonList;
    node->color = 'R';
    node->self_node = NULL;
    node->number_of_words = 0;
    node->word_id = 0;

    return node;
}

/* Left rotate procedure */
void LeftRotate(struct RBTree *T, struct node *x)
{
    struct node *y = x->right; /* set y */
    x->right = y->left;        /* turn y's left subtree into x???s right subtree */

    if (y->left != T->NIL) /* update parent pointer of y's left */
        y->left->parent = x;

    y->parent = x->parent; /* link x's parent to y */

    if (x->parent == T->NIL) /* x is root */
        T->root = y;
    else if (x == x->parent->left) /* x is left child */
        x->parent->left = y;
    else /* x is right child */
        x->parent->right = y;

    y->left = x;   /* put x on y's left */
    x->parent = y; /* update parent pointer of x */
}

/* Right rotate procedure */
void RightRotate(struct RBTree *T, struct node *y)
{
    struct node *x = y->left;
    y->left = x->right;

    if (x->right != T->NIL)
        x->right->parent = y;

    x->parent = y->parent;

    if (x->parent == T->NIL)
        T->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    x->right = y;
    y->parent = x;
}

/* RBTinsert calls RBTinsertFixup which secures Red Black Tree properties aren't violated */
void RBTinsertFixup(struct RBTree *T, struct node *z)
{
    while (z->parent->color == 'R')
    {
        /* CASE 1: Parent of z is in a left subtree */
        if (z->parent == z->parent->parent->left)
        {
            struct node *y = z->parent->parent->right;
            /* CASE 1.1: Aunt node y of z is red */
            if (y->color == 'R')
            {
                z->parent->color = 'B';         /* After Colorflip: */
                y->color = 'B';                 /* RED */
                z->parent->parent->color = 'R'; /*  BLACK   BLACK */
                z = z->parent->parent;
            }
            /* CASE 1.2: Aunt node y of z is black */
            /* There are 2 cases : RightRotate or LeftRightRotate */
            /* After rotatation: */
            /*        BLACK */
            /*      RED     RED */
            else
            {
                /* CASE 1.2.1 : First RightRotate and then LeftRotate  */
                /* z is a right child */
                if (z == z->parent->right)
                {
                    z = z->parent;
                    LeftRotate(T, z);
                }
                /* CASE 1.2.2: RightRotate */
                /* z is a left child */
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                RightRotate(T, z->parent->parent);
            }
        }
        /* CASE 2: Parent of z is in a right subtree */
        /* Similar to CASE 1 */
        else
        {
            struct node *y = z->parent->parent->left;

            if (y->color == 'R')
            {
                z->parent->color = 'B';
                y->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    RightRotate(T, z);
                }

                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                LeftRotate(T, z->parent->parent);
            }
        }
    }

    T->root->color = 'B';
}

/* Inseting node to RedBlackTree */
int RBTinsert(struct RBTree *T, struct node *z)
{

    struct node *y = T->NIL;
    struct node *x = T->root;

    /* Traverse the tree and find appropriate position for new node */
    while (x != T->NIL)
    {
        y = x;

        if (strcmp(z->key, x->key) < 0)
            x = x->left;
        else if (strcmp(z->key, x->key) > 0)
            x = x->right;
        else
            return 0;
    }

    z->parent = y;

    if (y == T->NIL)
        T->root = z;
    else
    {
        if (strcmp(z->key, y->key) <= 0)
            y->left = z;
        else
            y->right = z;
    }

    z->right = T->NIL;
    z->left = T->NIL;

    RBTinsertFixup(T, z);
    return 1;
}


/* insert node in bow or tf */
int RBTinsert_bow_tf(struct RBTree *T, struct node *z, int total_files)
{

    struct node *y = T->NIL;
    struct node *x = T->root;

    /* Traverse the tree and find appropriate position for new node */
    while (x != T->NIL)
    {
        y = x;

        if (strcmp(z->key, x->key) < 0)
            x = x->left;
        else if (strcmp(z->key, x->key) > 0)
            x = x->right;
        else
        {
            //check if we are in the same file and w already have counted it
            if (global_index != atoi(x->list_same_jsons->end->json_name))
            {
                char *temp_name = malloc(20);
                sprintf(temp_name, "%d", global_index);
                struct lnode *temp = new_lnode(temp_name);
                free(temp_name);
                insert_lnode(x->list_same_jsons, temp);
            }

            return 0;
        }
    }

    /* we use this list to know in how many files a word appears */
    /* the size of the list is the number of the files it appears to*/
    z->list_same_jsons = new_list();
    char *temp_name = malloc(20);
    sprintf(temp_name, "%d", global_index);
    struct lnode *temp = new_lnode(temp_name);
    free(temp_name);
    insert_lnode(z->list_same_jsons, temp);

    z->parent = y;

    if (y == T->NIL)
        T->root = z;
    else
    {
        if (strcmp(z->key, y->key) <= 0)
            y->left = z;
        else
            y->right = z;
    }

    z->right = T->NIL;
    z->left = T->NIL;

    RBTinsertFixup(T, z);
    return 1;
}

/* TRANSPLANT */
/* node v is the brother of deleted node u */
/* v will replace u */
/* v is being updated */
void transplant(struct RBTree *T, struct node *u, struct node *v)
{
    if (u->parent == T->NIL)
        T->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

/* Returns node with minimum value in the subtree with root x */
struct node *minimum(struct RBTree *T, struct node *x)
{
    while (x->left != T->NIL)
        x = x->left;
    return x;
}

/* inserts first tree's nodes to the second one*/
void combine_trees(struct RBTree *Tree1, struct node *recursion_root, struct RBTree *Tree2)
{
    if (recursion_root == Tree1->NIL)
    {
        return;
    }

    combine_trees(Tree1, recursion_root->left, Tree2);
    combine_trees(Tree1, recursion_root->right, Tree2);

    struct node *Newnode;
    Newnode = new_node(recursion_root->key, NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    Newnode->self_node = recursion_root->self_node;

    int res = -1;
    res = RBTinsert(Tree2, Newnode);
    if (res == 0)
    {
        free(Newnode->key);
        free(Newnode);
    }
}


// works like destroyRBTree but for different kind of tree nodes
void destroy_diffRBTree(struct RBTree *T, struct node *recursion_root)
{
    if (recursion_root == T->NIL && recursion_root != T->root)  // end of recursion case
        return;
    else if (recursion_root == T->root && T->root == T->NIL)    //tree is empty
    {
        free(T->NIL);
        free(T->directory_name);
        free(T);
        return;
    }

    destroy_diffRBTree(T, recursion_root->left);
    destroy_diffRBTree(T, recursion_root->right);

    if (recursion_root == T->root)
    {

        free(T->NIL);
        free(T->directory_name);
        free(T);
    }

    free(recursion_root->key);

    if(recursion_root->list_same_jsons!=NULL){

        delete_list(recursion_root->list_same_jsons);
        free(recursion_root->list_same_jsons);
    }

    free(recursion_root);
}



/*  Traversing tree in postorder and deleting each node */
void destroyRBTree(struct RBTree *T, struct node *recursion_root)
{
    if (recursion_root == T->NIL)
        return;

    destroyRBTree(T, recursion_root->left);
    destroyRBTree(T, recursion_root->right);

    if (recursion_root == T->root)
    {
        free(T->NIL);
        free(T->directory_name);
        free(T);
    }

    free(recursion_root->key);
    free(recursion_root->bow);
    free(recursion_root->tf_idf);
    free(recursion_root->non_zero_values);
    if (recursion_root->json_info != NULL)
        delete_json_list(recursion_root->json_info);
    free(recursion_root->json_info);

    if (recursion_root->list_same_jsons != NULL && recursion_root->list_same_jsons->size != -1)
    {
        delete_list_node(recursion_root->list_same_jsons);
        if (recursion_root->list_same_jsons->size == 0)
        {
            destroy_diffRBTree(recursion_root->list_same_jsons->different_cliques, recursion_root->list_same_jsons->different_cliques->root);
            destroy_diffRBTree(recursion_root->list_same_jsons->printed_different_cliques, recursion_root->list_same_jsons->printed_different_cliques->root);
            free(recursion_root->list_same_jsons);
        }
        recursion_root->list_same_jsons = NULL;
    }

    free(recursion_root);
}

void destroyRBTree_cloned(struct RBTree *T, struct node *recursion_root)
{
    if (recursion_root == T->NIL)
        return;

    destroyRBTree_cloned(T, recursion_root->left);
    destroyRBTree_cloned(T, recursion_root->right);

    if (recursion_root == T->root)
    {
        free(T->NIL);
        free(T->directory_name);
        free(T);
    }

    free(recursion_root->key);

    if (recursion_root->list_same_jsons != NULL && recursion_root->list_same_jsons->size != -1)
    {
        delete_list_node(recursion_root->list_same_jsons);
        if (recursion_root->list_same_jsons->size == 0)
        {
            destroy_diffRBTree(recursion_root->list_same_jsons->different_cliques, recursion_root->list_same_jsons->different_cliques->root);
            destroy_diffRBTree(recursion_root->list_same_jsons->printed_different_cliques, recursion_root->list_same_jsons->printed_different_cliques->root);
            free(recursion_root->list_same_jsons);
        }
        recursion_root->list_same_jsons = NULL;
    }

    free(recursion_root);
}

/* Returns node with specidied key */
/* If not found return NULL */
struct node *find_key_RBtree(struct RBTree *T, char *key)
{
    if (T == NULL || T->root == T->NIL) /* T == NULL in case product doesnt exist */
        return NULL;
    struct node *temp = T->root;

    while (temp != T->NIL)
    {
        if (strcmp(temp->key, key) == 0)
            return temp;

        else if (strcmp(temp->key, key) < 0) /* temp->voter->id < key, go right */
            temp = temp->right;
        else /* go left */
            temp = temp->left;
    }

    return NULL;
}


/* prints all different relations between the cliques */
void print_different(list *clique, struct RBTree *Tree_different_cliques, struct node *recursion_root, HashTable * files)
{
    if (recursion_root == Tree_different_cliques->NIL)
        return;
    print_different(clique, Tree_different_cliques, recursion_root->right,files);
    print_different(clique, Tree_different_cliques, recursion_root->left, files);

    struct node *check = find_key_RBtree(clique->printed_different_cliques, recursion_root->self_node->list_same_jsons->start->json_name);
    struct node *check2 = find_key_RBtree(recursion_root->self_node->list_same_jsons->printed_different_cliques, clique->start->json_name);
    if (check == NULL && check2 == NULL)        // check if we already have printing the given relation
    {
        // store in trees so we dont print the same relation more than one time
        print_two_lists(clique, recursion_root->self_node->list_same_jsons,files);
        int res = -1;
        struct node *temp1 = new_node(recursion_root->self_node->list_same_jsons->start->json_name, NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
        res = RBTinsert(clique->printed_different_cliques, temp1);
        if (res == 0)
        {
            free(temp1->key);
            free(temp1);
        }
        res = -1;
        temp1 = new_node(clique->start->json_name, NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
        res = RBTinsert(recursion_root->self_node->list_same_jsons->different_cliques, temp1);
        if (res == 0)
        {
            free(temp1->key);
            free(temp1);
        }
    }
}


/* Print matching json files */
void postorder_print_commons(struct RBTree *T, struct node *node, HashTable * files)
{
    if (node != T->NIL)
    {
        postorder_print_commons(T, node->left,files);
        postorder_print_commons(T, node->right,files);

        if (node->list_same_jsons->print_flag == 0 && node->list_same_jsons->size > 1)
            print_list(node->list_same_jsons,files);
        
        print_different(node->list_same_jsons, node->list_same_jsons->different_cliques, node->list_same_jsons->different_cliques->root,files);

    }
}

void postorder_remove_duplicates(struct RBTree *T, struct node *node)
{
    if (node != T->NIL)
    {
        postorder_remove_duplicates(T, node->left);
        postorder_remove_duplicates(T, node->right);

        if (node->list_same_jsons->Removed_duplicates == 0)
        {
            struct RBTree *new_tree = new_RBTree("Different_cliques");
            fix_duplicates(node->list_same_jsons->different_cliques, node->list_same_jsons->different_cliques->root, new_tree);
            destroy_diffRBTree(node->list_same_jsons->different_cliques, node->list_same_jsons->different_cliques->root);
            node->list_same_jsons->different_cliques = new_tree;
        }
    }
}

void postorder_print_different(struct RBTree *Tree, struct node *node, HashTable * files)
{

    if (node != Tree->NIL)
    {
        postorder_print_different(Tree, node->left,files);
        postorder_print_different(Tree, node->right,files);

        print_different(node->list_same_jsons, node->list_same_jsons->different_cliques, node->list_same_jsons->different_cliques->root, files);
    }
}

/* Inspired by https://www.techiedelight.com/determine-binary-tree-satisfy-height-balanced-property-red-black-tree/ */

/* Recursive function to determine if the given binary tree */
/* satisfy the height-balanced property of red???black tree or not */
/* It takes reference to rootMax variable for storing the */
/* maximum height of the root node */
int isHeightBalanced(struct RBTree *tree, struct node *root, int *rootMax)
{
    /* Base case */
    if (root == tree->NIL)
        return 1;

    /* variables to hold maximum height of left and right subtree */
    int leftMax = 0, rightMax = 0;

    /* proceed only if both left and right subtree are balanced */
    if (isHeightBalanced(tree, root->left, &leftMax) &&
        isHeightBalanced(tree, root->right, &rightMax))
    {
        /* Calculate the minimum height of the left and right subtree */
        int rootMin;
        if (leftMax <= rightMax)
            rootMin = leftMax + 1;
        else
            rootMin = rightMax + 1;

        /* Calculate the maximum height of the left and right subtree */
        if (leftMax >= rightMax)
            *rootMax = leftMax + 1;
        else
            *rootMax = rightMax + 1;

        /* return 1 if the root node is height balanced */
        if (*rootMax <= 2 * rootMin)
            return 1;
    }

    /* return 0 if either left or right subtree is unbalanced */
    return 0;
}

void initialize_bow_tf_idf(struct node *file, HashTable *diffWords)     //initializes bow and tf_idf arrays
{
    file->bow = malloc(sizeof(int) * global_total_words);           //we allocate space for the arrays
    file->tf_idf = malloc(sizeof(double) * global_total_words);

    /* Initialize both arrays with 0 */
    memset(file->bow, 0, sizeof(int) * global_total_words);
    memset(file->tf_idf, 0, sizeof(double) * global_total_words);

    json_node *temp_category = file->json_info->start;
    lnode *temp_value = temp_category->values->start;

    int index;
    int count = 0;
    double tf, idf;
    struct node *temp;
    // here we store for each word of the file it's index in real bow array
    int *word_ids_array = malloc(sizeof(int) * file->number_of_words);
    memset(word_ids_array, 0, sizeof(int) * file->number_of_words);

    // we use this tree to ignore duplicates
    struct RBTree * sorted_non_zero_values = new_RBTree("sorted_non_zero_values");
    char * id_key = malloc(30);
    struct node * id ;
    int res=-1;
    
    
    while (temp_category != NULL)       //for each category of the list
    {
        temp_value = temp_category->values->start;
        while (temp_value != NULL)      //for each value of each category
        {
            index = hash1(temp_value->json_name, diffWords->size);
            temp = find_key_RBtree(diffWords->Trees[index], temp_value->json_name);     //find the word in the tree of different words
            file->bow[temp->word_id] += 1;      //increase the word's count by 1 in bow
            
            sprintf(id_key, "%d",temp->word_id);
            id = new_node(id_key, NULL, NO_PARAMETER, NO_PARAMETER);      //try to insert a node in the tree
            res = RBTinsert(sorted_non_zero_values, id);
            if(res==1)      //new word so we store it's position and increase the counter
            {
                word_ids_array[count] = temp->word_id;
                count++;
            }
            else        //word already in the tree
            {
                free(id->key);
                free(id);
            }
            
            file->tf_idf[temp->word_id] = temp->list_same_jsons->size; //store temporarily how many times a word appears to files (idf)
            temp_value = temp_value->next;
        }
        temp_category = temp_category->next;
    }

    temp_category = file->json_info->start;
    temp_value = temp_category->values->start;
    for (int i = 0; i < file->number_of_words; i++)    //initialize tfidf array
    {
        if (word_ids_array[i] != 0)
        {
            tf = ((double)file->bow[word_ids_array[i]]) / ((double)file->number_of_words);  //calculate tf
            idf = log((double)global_index / file->tf_idf[word_ids_array[i]]);          //calculate idf
            file->tf_idf[word_ids_array[i]] = tf * idf;                                     //store the value in tfidf array
        }
        else        //when we find zero there are no other words so we stop
            break;
    }



    free(id_key);

    file->non_zero_values = word_ids_array;
    destroy_diffRBTree(sorted_non_zero_values,sorted_non_zero_values->root);
}

void postorder_initialize_bow_tfidf(struct RBTree *Tree, struct node *root, HashTable *diffWords)
{
    if (root == Tree->NIL)
        return;

    postorder_initialize_bow_tfidf(Tree, root->left, diffWords);
    postorder_initialize_bow_tfidf(Tree, root->right, diffWords);

    initialize_bow_tf_idf(root, diffWords);
}



void postorder_getAllRecords(struct RBTree * Tree, struct node * root, HashTable * newTable)
{
    
    if(root == Tree->NIL)
        return ;
    
    postorder_getAllRecords(Tree, root->right,newTable);
    postorder_getAllRecords(Tree, root->left, newTable);
    
    insert_Record_clone(root->key, newTable, root->json_info, 0,root);
    
}








void postorder_checkifElementinDifferentCliques(struct RBTree * Tree, struct node * root , char * element,int * flag )
{
    if (root == Tree->NIL ||*flag==1)
        return ;
    
    
    if(strcmp(root->key, element)==0)
    {
        *flag=1;
    }
    
    
    
    postorder_checkifElementinDifferentCliques(Tree,root->left,element,flag);
    postorder_checkifElementinDifferentCliques(Tree,root->right,element,flag);
    
    

}


void postorder_findCliques_conflicts(struct RBTree *T, struct node *node, HashTable * files ,logistic_regression * model)
{
    if (node == T->NIL)
        return ;

    postorder_findCliques_conflicts(T, node->left,files,model);
    postorder_findCliques_conflicts(T, node->right,files,model);
    
    int flag=0;
    
    if (node->list_same_jsons->print_flag == 0 && node->list_same_jsons->size > 1)
    {
        
        node->list_same_jsons->print_flag=1;
        lnode * start = node->list_same_jsons->start;
        
        //if some element of one clique is same with an element in different-element-tree then there is conflict
        while(start!=NULL)
        {
            postorder_checkifElementinDifferentCliques(node->list_same_jsons->different_cliques,
                                                       node->list_same_jsons->different_cliques->root, start->json_name, &flag);

            if(flag==1)
                break;

            start = start->next;
        }
        if(flag==1) {
            conflicts += node->list_same_jsons->size;
            fixConflicts(files, node->list_same_jsons, model);
        }
    }
    
}


//unit-test function / same logic as the other function but doesnt use global variables

void postorder_findCliques_conflicts_test(struct RBTree *T, struct node *node, HashTable * files ,logistic_regression * model, int * flag)
{
    if (node == T->NIL)
        return ;
    
    postorder_findCliques_conflicts_test(T, node->left,files,model,flag);
    postorder_findCliques_conflicts_test(T, node->right,files,model,flag);
    
    
    if (node->list_same_jsons->print_flag == 0 && node->list_same_jsons->size > 1)
    {
        
        node->list_same_jsons->print_flag=1;
        lnode * start = node->list_same_jsons->start;
        while(start!=NULL)
        {
            postorder_checkifElementinDifferentCliques(node->list_same_jsons->different_cliques,
                                                       node->list_same_jsons->different_cliques->root, start->json_name, flag);
            
            if(*flag==1)
                break;
            
            start = start->next;
        }
        if(*flag==1) {
            return ;
        }
    }
    
}



