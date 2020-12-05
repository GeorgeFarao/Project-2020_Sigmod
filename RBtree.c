/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"

/* Creating new RedBlackTree */
struct RBTree * new_RBTree(char * directory_name )
{
    struct RBTree * Tree = malloc(sizeof(struct RBTree));
    struct node * nil_node = malloc(sizeof(struct node));
    
    
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
struct node * new_node( char * json_id, json_list * jsonList)
{
    struct node * node = malloc(sizeof(struct node));
    
    node->key=malloc( strlen(json_id)+1 );
    strcpy(node->key, json_id);

    if(jsonList!=NULL)
    {
        node->list_same_jsons = new_list();     /* List with matching json files */

        /* Initialize list with itself */
        lnode *listnode = new_lnode(json_id);
        insert_lnode(node->list_same_jsons, listnode);
        node->list_same_jsons->different_cliques = new_RBTree("Tree_For_Different_CLiques");
    } else{
        node->list_same_jsons=NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->json_info=jsonList;
    node->color = 'R';
    node->self_node=NULL;
    


    
    
    return node;
}

/* Left rotate procedure */
void LeftRotate(struct RBTree * T, struct node * x)
{
    struct node * y = x->right; /* set y */
    x->right = y->left; /* turn y's left subtree into x’s right subtree */
    
    if(y->left != T->NIL) /* update parent pointer of y's left */
        y->left->parent = x;
    
    y->parent = x->parent; /* link x's parent to y */
    
    if(x->parent == T->NIL)  /* x is root */
        T->root = y;
    else if(x == x->parent->left)  /* x is left child */
        x->parent->left = y;
    else  /* x is right child */
        x->parent->right = y;
    
    y->left = x;  /* put x on y's left */
    x->parent = y;  /* update parent pointer of x */
}

/* Right rotate procedure */
void RightRotate(struct RBTree *T, struct node *y)
{
    struct node * x = y->left;
    y->left = x->right;
    
    if(x->right != T->NIL)
        x->right->parent = y;
    
    x->parent = y->parent;
    
    if(x->parent == T->NIL)
        T->root = x;
    else if(y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    
    x->right = y;
    y->parent = x;
}

/* RBTinsert calls RBTinsertFixup which secures Red Black Tree properties aren't violated */
void RBTinsertFixup(struct RBTree * T, struct node * z)
{
    while (z->parent->color == 'R')
    {
        /* CASE 1: Parent of z is in a left subtree */
        if(z->parent == z->parent->parent->left)
        {
            struct node *y = z->parent->parent->right;
            /* CASE 1.1: Aunt node y of z is red */
            if(y->color == 'R')
            {
                z->parent->color = 'B'; /* After Colorflip: */
                y->color = 'B';                         /* RED */
                z->parent->parent->color = 'R';   /*  BLACK   BLACK */
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
            struct node * y = z->parent->parent->left;
            
            if(y->color == 'R')
            {
                z->parent->color = 'B';
                y->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            }
            else
            {
                if(z == z->parent->left)
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
int RBTinsert(struct RBTree * T, struct node * z)
{
    
    struct node * y = T->NIL;
    struct node * x = T->root;
    
    /* Traverse the tree and find appropriate position for new node */
    while(x != T->NIL)
    {
        y = x;

        if(strcmp(z->key, x->key)<0)
            x = x->left;
        else if (strcmp(z->key, x->key)>0)
            x = x->right;
        else
            return 0;
        
    }
    
    z->parent = y;
    
    if (y == T->NIL)
        T->root = z;
    else
    {
        if(strcmp(z->key, y->key) <= 0)
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
void transplant(struct RBTree * T, struct node * u, struct node * v)
{
    if(u->parent == T->NIL)
        T->root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    
    v->parent = u->parent;
}


/* Returns node with minimum value in the subtree with root x */
struct node * minimum(struct RBTree * T, struct node * x)
{
    while(x->left != T->NIL)
        x = x->left;
    return x;
}







void combine_trees(struct RBTree * Tree1, struct node * recursion_root , struct RBTree * Tree2 )
{
    if(recursion_root== Tree1->NIL) {
        return;
    }
    
    combine_trees(Tree1, recursion_root->left, Tree2);
    combine_trees(Tree1, recursion_root->right, Tree2);

    struct node * Newnode;
    Newnode = new_node(recursion_root->key, NULL);
    Newnode->self_node=recursion_root->self_node;
    

    int res=-1;
    res=  RBTinsert(Tree2, Newnode);
    if(res==0){
        free(Newnode->key);
        free(Newnode);
    }
}



void destroy_diffRBTree(struct RBTree * T, struct node * recursion_root)
{
    if (recursion_root == T->NIL && recursion_root !=T->root  )
        return;
    else  if(recursion_root == T->root && T->root== T->NIL)
    {
        free(T->NIL);
        free(T->directory_name);
        free(T);
        return ;
    }


    destroy_diffRBTree(T, recursion_root->left );
    destroy_diffRBTree(T, recursion_root->right);

    if(recursion_root == T->root )
    {

        free(T->NIL);
        free(T->directory_name);
        free(T);

    }
    free(recursion_root->key);

    free(recursion_root);

}





/*  Traversing tree in postorder and deleting each node */
void destroyRBTree(struct RBTree * T, struct node * recursion_root)
{
    if (recursion_root == T->NIL )
        return;
        
    destroyRBTree(T, recursion_root->left );
    destroyRBTree(T, recursion_root->right);


    if(recursion_root == T->root)
    {
        free(T->NIL);
        free(T->directory_name);
        free(T);
    }

    free(recursion_root->key);

    if (recursion_root->json_info != NULL)
        delete_json_list(recursion_root->json_info);
    free(recursion_root->json_info);

    if( recursion_root->list_same_jsons!=NULL && recursion_root->list_same_jsons->size!=-1)
    {
        delete_list_node(recursion_root->list_same_jsons);
        if(recursion_root->list_same_jsons->size==0) {
            destroy_diffRBTree(recursion_root->list_same_jsons->different_cliques, recursion_root->list_same_jsons->different_cliques->root);
            free(recursion_root->list_same_jsons);
        }
        recursion_root->list_same_jsons=NULL;
        
    }
    
    
    free(recursion_root);

}




/* Returns node with specidied key */
/* If not found return NULL */
struct node * find_key_RBtree(struct RBTree *T, char *key)
{
    if(T == NULL || T->root==T->NIL) /* T == NULL in case product doesnt exist */
        return NULL;
    struct node* temp = T->root;

    while(temp != T->NIL)
    {
        if(strcmp(temp->key ,key) == 0)
            return temp;
        
        else if(strcmp(temp->key, key) < 0) /* temp->voter->id < key, go right */
            temp = temp->right;
        else /* go left */
            temp = temp->left;
    }

    return NULL;
    
}

/* Print matching json files */
void postorder_print_commons(struct RBTree *T, struct node *node)
{
    if (node != T->NIL)
    {
        postorder_print_commons(T, node->left);
        postorder_print_commons(T, node->right);
        
        if ( node->list_same_jsons->print_flag==0 && node->list_same_jsons->size >1 )
            print_list(node->list_same_jsons);

    }
}


/* Inspired by https://www.techiedelight.com/determine-binary-tree-satisfy-height-balanced-property-red-black-tree/ */

/* Recursive function to determine if the given binary tree */
/* satisfy the height-balanced property of red–black tree or not */
/* It takes reference to rootMax variable for storing the */
/* maximum height of the root node */
int isHeightBalanced(struct RBTree * tree, struct node *root, int *rootMax)
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
        if (*rootMax <= 2*rootMin)
            return 1;
    }
 
    /* return 0 if either left or right subtree is unbalanced */
    return 0;
}
