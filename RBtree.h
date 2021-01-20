/* Authors Kazakos Vasileios , Farao Georgios , Manolis Stivaktas */

#define NO_PARAMETER 0
#define DIFFERENT_CLIQUES 1
#define BOW_TF_IDF 2
#define MINI_BATCH_M 512
#define NUMBER_OF_THREADS 8
#define TRAINING 0
#define TESTING 1
#define TERMINATE 2
#define NOJOB 3
#define VALIDATION_MAX_SIZE 200000000

extern int global_index;
extern int global_total_words;

struct node
{
    char color;                         /* for color property */
    struct node *left, *right, *parent; /* links for left, right children and parent */
    char *key;                          /* json file */
    list *list_same_jsons;              /* Pointer to list with matching json files of current json */
    struct node *self_node;

    json_list *json_info; /* List of categories and values of each category of json file */
    int *bow;
    double *tf_idf;
    int * non_zero_values;
    
    int bow_tf_idf_index;
    int number_of_words;
    int word_id;
    
    
    
};

struct RBTree
{
    struct node *root;
    struct node *NIL;
    char *directory_name;
};

/////////////////////////////////////////   FUNCTIONS   /////////////////////////////////////////

struct RBTree *new_RBTree(char *directory_name); /* Creating new RedBlackTree */

struct node *new_node(char *json_id, json_list *jsonList, int flag, int total_files); /* Creating new tree node */

void LeftRotate(struct RBTree *T, struct node *x); /* Left rotate procedure */

void RightRotate(struct RBTree *T, struct node *y); /* Right rotate procedure */

/* RBTinsert calls RBTinsertFixup which secures Red Black Tree properties aren't violated */
void RBTinsertFixup(struct RBTree *T, struct node *z);

int RBTinsert(struct RBTree *T, struct node *z); /* Inseting node to RedBlackTree */

/* TRANSPLANT */
/* node v is the brother of deleted node u */
/* v will replace u */
/* v is being updated */
void transplant(struct RBTree *T, struct node *u, struct node *v);

/* Returns node with minimum value in the subtree with root x */
struct node *minimum(struct RBTree *T, struct node *x);

/*   Combine Trees */
void combine_trees(struct RBTree *Tree1, struct node *recursion_root, struct RBTree *Tree2);

void destroy_diffRBTree(struct RBTree *T, struct node *recursion_root);
/*  Traversing tree in postorder and deleting each node */
void destroyRBTree(struct RBTree *T, struct node *node);

/* Returns node with specidied key */
/* If not found return NULL */
struct node *find_key_RBtree(struct RBTree *T, char *key);


void postorder_remove_duplicates(struct RBTree *T, struct node *node);

/* Check if height is balanced - Useful for unit testing */
int isHeightBalanced(struct RBTree *tree, struct node *root, int *rootMax);

int RBTinsert_bow_tf(struct RBTree *T, struct node *z, int total_files);
void postorder_checkifElementinDifferents(struct RBTree  * Tree, struct node * root,char * element,int * flag);
