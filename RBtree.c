//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include "RBtree.h"




struct node * new_node( char * json_id, json_list * jsonList)
{
    struct node * node = malloc(sizeof(struct node));
    
    node->key=malloc( strlen(json_id)+1 );
    strcpy(node->key, json_id);
    
    node->list_same_jsons = new_list();     //we create a list
    
    lnode * listnode = new_lnode(json_id);      //and we add same json_id to new list
    insert_lnode(node->list_same_jsons, listnode);
    
    
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->json_info=jsonList;
    node->color = 'R';
    
    return node;
}


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

void LeftRotate(struct RBTree * T, struct node * x)
{
    struct node * y = x->right; //set y
    x->right = y->left; //turn y's left subtree into x’s right subtree
    
    if(y->left != T->NIL) //update parent pointer of y's left
        y->left->parent = x;
    
    y->parent = x->parent; // link x's parent to y
    
    if(x->parent == T->NIL)  //x is root
        T->root = y;
    else if(x == x->parent->left)  //x is left child
        x->parent->left = y;
    else  //x is right child
        x->parent->right = y;
    
    y->left = x;  // put x on y's left
    x->parent = y;  //update parent pointer of x
}

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



void RBTinsertFixup(struct RBTree * T, struct node * z)
{
    while (z->parent->color == 'R')
    {
        //CASE 1: Parent of z is in a left subtree
        if(z->parent == z->parent->parent->left)
        {
            struct node *y = z->parent->parent->right;
            // CASE 1.1: Aunt node y of z is red
            if(y->color == 'R')
            {
                z->parent->color = 'B'; // After Colorflip:
                y->color = 'B';           //        RED
                z->parent->parent->color = 'R';   //  BLACK   BLACK
                z = z->parent->parent;
            }
            // CASE 1.2: Aunt node y of z is black
            //There are 2 cases : RightRotate or LeftRightRotate
            //After rotatation:
            //        BLACK
            //      RED     RED
            else
            {
                // CASE 1.2.1 : First RightRotate and then LeftRotate // z is a right child
                if (z == z->parent->right)
                {
                    z = z->parent;
                    LeftRotate(T, z);
                }
                // CASE 1.2.2: RightRotate // z is a left child
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                RightRotate(T, z->parent->parent);
            }
        }
        //CASE 2: Parent of z is in a right subtree
        //Similar to CASE 1
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


void RBTinsert(struct RBTree * T, struct node * z)
{
    
    struct node * y = T->NIL;
    struct node * x = T->root;
    
    while(x != T->NIL)
    {
        y = x;

        if(strcmp(z->key, x->key)<=0)
            x = x->left;
        else
            x = x->right;
        
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
}

// TRANSPLANT
// node v is the brother of deleted node u
// v will replace u
// v is being updated
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

// returns node with minimum value in the subtree with root x
struct node * minimum(struct RBTree * T, struct node * x)
{
    while(x->left != T->NIL)
        x = x->left;
    return x;
}


// Traversing tree in postorder and deleting each node
void destroyRBTree(struct RBTree * T, struct node * recursion_root)
{

    
    if (recursion_root == T->NIL)
        return;
        
    destroyRBTree(T, recursion_root->left );
    destroyRBTree(T, recursion_root->right);
        

    
    if(recursion_root == T->root)
    {
        free(T->NIL);
        free(T->directory_name);
        //free(T->root);
        free(T);
    }

    free(recursion_root->key);
    
    free(recursion_root);

}

struct node * find_key_RBtree(struct RBTree *T, char *key)
{
    //T==NULL in case product doesnt exist
    if(T==NULL || T->root==T->NIL)
        return NULL;
    struct node* temp = T->root;

    while(temp != T->NIL)
    {
        if(strcmp(temp->key ,key) == 0)
            return temp;
        
        else if(strcmp(temp->key, key) < 0) //temp->voter->id < key, go right
            temp = temp->right;
        else //go left
            temp = temp->left;
    }

    return NULL;
    
}

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

