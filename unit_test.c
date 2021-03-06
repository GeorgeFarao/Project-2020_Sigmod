#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "list.h"
#include "jsonParser.h"
#include "RBtree.h"
#include "HashTable.h"
#include "helpFunctions.h"
#include "acutest.h"
#include "dataList.h"
#include "logistic_regression.h"


void test_RBTree(void)
{
    /* Creating RBTree and inserting nodes with json_id's as the given sequence of numbers below */
    /* 8, 18, 5, 15, 17, 25, 40, 80 */
    struct RBTree *tree = new_RBTree("www.ebay.com");
    int rootMax = 0;

    /* Creating nodes */
    struct node *tree_node1 = new_node("8.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node2 = new_node("18.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node3 = new_node("5.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node4 = new_node("15.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node5 = new_node("17.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node6 = new_node("25.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node7 = new_node("40.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);
    struct node *tree_node8 = new_node("80.json", NULL, DIFFERENT_CLIQUES, NO_PARAMETER);

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

void test_data_list(void)
{
    train_data * data  = new_train_data("file1", "file2", 0);
    train_data * data2 = new_train_data("file4", "file2", 1);
    
    list_data  *temp_list = new_list_data();
    lnode_data * temp_node1 = new_lnode_data(data);
    lnode_data * temp_node2 = new_lnode_data(data2);
    
    insert_lnode_data(temp_list, temp_node1);
    insert_lnode_data(temp_list, temp_node2);
    
    /* Check for creation of list */
    TEST_CHECK(temp_list != NULL);
    TEST_MSG("List is null after creation.\n");
    
    
    /* Check for creation of lnodes */
    TEST_CHECK(temp_node1 != NULL);
    TEST_MSG("lnode1 is null after creation.\n");
    TEST_CHECK(temp_node2 != NULL);
    TEST_MSG("lnode2 is null after creation.\n");
    
    /* Check for insertion */
    TEST_CHECK(temp_list->start == temp_node1 && temp_list->end == temp_node2);
    TEST_MSG("Something went wrong with list insertion.");
    
    
    
}



void test_list(void)
{
    list *temp_list = new_list();
    lnode *temp_lnode1 = new_lnode("1.json");
    lnode *temp_lnode2 = new_lnode("2.json");

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

void test_hashtable(void)
{
    
    
    HashTable *table = newHashTable(15);
    int hash;
    json_list *temp = new_json_list();
    json_node *temp_jnode1, *temp_jnode2;

    temp_jnode1 = new_json_node("first_cat");
    temp_jnode2 = new_json_node("second_cat");
    add_category_value(temp, "first_cat", "value1");
    add_category_value(temp, "second_cat", "value2");
    /* Check for creation of HashTable */
    TEST_CHECK(table != NULL);
    TEST_MSG("Table was not create.\n");

    /* Check for insertion */
    insert_Record("ebay100.json", table, temp, NO_PARAMETER);
    hash = hash1("ebay100.json", 15);

    TEST_CHECK(table->Trees[hash]->root != NULL);
    TEST_MSG("Insertion went bad\n");

    /*  Check for match_same_products function   */

    insert_Record("ebay101.json", table, temp, NO_PARAMETER);
    insert_Record("ebay102.json", table, temp, NO_PARAMETER);
    insert_Record("ebay103.json", table, temp, NO_PARAMETER);
    insert_Record("diaforetikos", table, temp, NO_PARAMETER);

    match_same_products(table, "ebay101.json", "ebay102.json"); //101 = 102
    match_same_products(table, "ebay103.json", "ebay100.json"); //103 = 100
    match_same_products(table, "ebay101.json", "ebay100.json"); //101 = 100 => 101=102=103=100
    
    match_different_products(table, "diaforetikos", "ebay100.json");
    

    struct node *ebay100 = find_key_RBtree(table->Trees[hash1("ebay100.json", 15)], "ebay100.json");
    struct node *ebay101 = find_key_RBtree(table->Trees[hash1("ebay101.json", 15)], "ebay101.json");
    struct node *ebay102 = find_key_RBtree(table->Trees[hash1("ebay102.json", 15)], "ebay102.json");
    struct node *ebay103 = find_key_RBtree(table->Trees[hash1("ebay103.json", 15)], "ebay103.json");

    TEST_CHECK(ebay101->list_same_jsons == ebay102->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");

    TEST_CHECK(ebay100->list_same_jsons == ebay103->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");

    TEST_CHECK(ebay101->list_same_jsons == ebay103->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");
    
    TEST_CHECK(   strcmp(ebay101->list_same_jsons->different_cliques->root->key , "diaforetikos")==0   );
    TEST_MSG("Nodes are not different\n");
    
    match_same_products(table, "diaforetikos", "ebay100.json");
    TEST_CHECK(find_conflicts_test(table, NULL)==1);
    TEST_MSG("Conflict not found \n");
    
    
    // delete_hashtable(table);
    free(table);
    free(temp);
}

TEST_LIST = {
    {"RBTree", test_RBTree},
    {"hash", test_hashtable},
    {"json_list", test_json_list},
    {"list", test_list},
    {"Data_List",test_data_list},
    {NULL, NULL}};
