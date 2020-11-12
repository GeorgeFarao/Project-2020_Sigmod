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

TEST_LIST = {
    {"list", test_list},
    {NULL, NULL}
};