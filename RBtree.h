//Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas

#include <stdlib.h>
#include <string.h>
#include "jsonParser.h"

struct node {
    char color;  // for color property
    struct node * left, * right, * parent; // links for left, right children and parent
    char * key; // entryDate or id
    list * list_same_jsons;
    json_list * json_info;
};

struct RBTree {
    struct node * root;
    struct node * NIL;
    char * directory_name;
};

///////////////////// FUNCTIONS //////////////////

struct node * find_key_RBtree(struct RBTree *T, char *key);

void postorder_print_commons(struct RBTree *T, struct node *node);


struct node * new_node( char * json_id, json_list *);

struct RBTree * new_RBTree(char * directory_name );

void LeftRotate(struct RBTree * T, struct node * x);

void RightRotate(struct RBTree * T, struct node * y);

void RBTinsertFixup(struct RBTree * T, struct node * z);

void RBTinsert(struct RBTree * T, struct node * z);

void transplant(struct RBTree * T, struct node * u, struct node * v);

struct node * minimum(struct RBTree * T, struct node * x);

void destroyRBTree(struct RBTree * T, struct node * node);


