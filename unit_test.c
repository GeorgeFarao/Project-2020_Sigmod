#include "acutest.h"
#include "HashTable.h"

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>


void test_RBTree (void)
{   
    /* Creating RBTree and inserting nodes with json_id's as the given sequence of numbers below */
    /* 8, 18, 5, 15, 17, 25, 40, 80 */
    struct RBTree * tree = new_RBTree("www.ebay.com");
    int rootMax = 0;

    /* Creating nodes */
    struct node * tree_node1 = new_node("8.json", NULL);
    struct node * tree_node2 = new_node("18.json", NULL);
    struct node * tree_node3 = new_node("5.json", NULL);
    struct node * tree_node4 = new_node("15.json", NULL);
    struct node * tree_node5 = new_node("17.json", NULL);
    struct node * tree_node6 = new_node("25.json", NULL);
    struct node * tree_node7 = new_node("40.json", NULL);
    struct node * tree_node8 = new_node("80.json", NULL);

    /* Inserting nodes */
    RBTinsert(tree, tree_node1);
    RBTinsert(tree, tree_node2);
    RBTinsert(tree, tree_node3);
    RBTinsert(tree, tree_node4);
    RBTinsert(tree, tree_node5);
    RBTinsert(tree, tree_node6);
    RBTinsert(tree, tree_node7);
    RBTinsert(tree, tree_node8);

    /* Check if height is balanced */
    TEST_CHECK(isHeightBalanced(tree, tree->root, &rootMax) == 1);
    TEST_MSG("Error in RBTree, height not balanced!");

    destroyRBTree(tree, tree->root);
}


void test_list (void)
{
    list * temp_list = new_list();
    lnode * temp_lnode1 = new_lnode("1.json");
    lnode * temp_lnode2 = new_lnode("2.json");

    insert_lnode(temp_list, temp_lnode1);
    insert_lnode(temp_list, temp_lnode2);

    /* Check for creation of list */
    TEST_CHECK(temp_list != NULL);    
    TEST_MSG("List is null after creation.\n");

    /* Check for creation of lnodes */
    TEST_CHECK(temp_lnode1 != NULL);    
    TEST_MSG("lnode1 is null after creation.\n");
    TEST_CHECK(temp_lnode2 != NULL);    
    TEST_MSG("lnode2 is null after creation.\n");    
    
    /* Check for insertion */
    TEST_CHECK(temp_list->start == temp_lnode1 && temp_list->end == temp_lnode2);
    TEST_MSG("Something went wrong with list insertion.");
    
    /* Check for deletion */
    delete_list_node(temp_list); /* Supposed to delete first node */
    TEST_CHECK(temp_list->start == temp_lnode2 && temp_list->end == temp_lnode2);
    TEST_MSG("Something went wrong with list node deletion.");
    delete_list_node(temp_list);
    TEST_CHECK(temp_list->size == 0); /* Size must be 0 */
    TEST_MSG("Something went wrong with list node deletion.");

    free(temp_list);
}

void test_json_list(void)
{
    json_list *temp_list = new_json_list();
    json_node *temp_jnode1, *temp_jnode2;

    temp_jnode1 = new_json_node("first_cat");
    temp_jnode2 = new_json_node("second_cat");

    /* Check for creation of list */
    TEST_CHECK(temp_list != NULL);
    TEST_MSG("List is null after creation.\n");

    /* Check for creation of jnodes */
    TEST_CHECK(temp_jnode1 != NULL);
    TEST_MSG("lnode1 is null after creation.\n");
    TEST_CHECK(temp_jnode2 != NULL);
    TEST_MSG("lnode2 is null after creation.\n");

    add_category_value(temp_list, "first_cat", "value1");
    add_category_value(temp_list, "second_cat", "value2");

    TEST_CHECK(strcmp(temp_list->start->category, "first_cat") == 0 &&
               strcmp(temp_list->end->category, "second_cat") == 0);
    TEST_MSG("Something went wrong with list of categories insertion.");

    TEST_CHECK(strcmp(temp_list->start->values->start->json_name, "value1") == 0 &&
               strcmp(temp_list->end->values->start->json_name, "value2") == 0);
    TEST_MSG("Something went wrong with list of values insertion.");

    delete_json_list(temp_list);
    TEST_CHECK(temp_list->start == NULL && temp_list->end == NULL);
    TEST_MSG("Something went wrong with list deletion.");

    free(temp_list);
    free(temp_jnode1->category);
    free(temp_jnode1->values);
    free(temp_jnode1);
    free(temp_jnode2->category);
    free(temp_jnode2->values);
    free(temp_jnode2);
}




TEST_LIST = {
    {"RBTree", test_RBTree},
    {"json_list", test_json_list},
    {"list", test_list},
    {NULL, NULL}
};
