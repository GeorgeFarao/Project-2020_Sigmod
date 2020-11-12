#include "acutest.h"
#include "HashTable.h"

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

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

void test_hashtable(void)
{
    HashTable * table = newHashTable(15);
    int hash;
    
    /* Check for creation of HashTable */
    TEST_CHECK(table!=NULL );
    TEST_MSG("Table was not create.\n");
    
    /* Check for insertion */
    insert_Record("ebay100.json", table, NULL);
    hash = hash1("ebay100.json", 15);
    
    TEST_CHECK(table->Trees[hash]->root!=NULL);
    TEST_MSG("Insertion went bad\n");
    
    /*  Check for match_same_products function   */
    
    insert_Record("ebay101.json", table, NULL);
    insert_Record("ebay102.json", table, NULL);
    insert_Record("ebay103.json", table, NULL);

    
    match_same_products(table, "ebay101.json", "ebay102.json");     //101 = 102
    match_same_products(table, "ebay103.json", "ebay100.json");     //103 = 100
    match_same_products(table, "ebay101.json", "ebay100.json");     //101 = 100 => 101=102=103=100
    
    
    struct node * ebay100 = find_key_RBtree(table->Trees[ hash1("ebay100.json", 15) ], "ebay100.json");
    struct node * ebay101 = find_key_RBtree(table->Trees[ hash1("ebay101.json", 15) ], "ebay101.json");
    struct node * ebay102 = find_key_RBtree(table->Trees[ hash1("ebay102.json", 15) ], "ebay102.json");
    struct node * ebay103 = find_key_RBtree(table->Trees[ hash1("ebay103.json", 15) ], "ebay103.json");
    
    
    TEST_CHECK(ebay101->list_same_jsons == ebay102->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");
    
    TEST_CHECK(ebay100->list_same_jsons == ebay103->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");
    
    TEST_CHECK(ebay101->list_same_jsons == ebay103->list_same_jsons);
    TEST_MSG("Nodes do not point at same clique \n");
    
    delete_hashtable(table);
    free(table);
    
}



TEST_LIST = {
    {"hash", test_hashtable},

    {"list", test_list},
    {NULL, NULL}
};
