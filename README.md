# **Software Development for Information Systems**


### Purpose:
Purpose of the project is to find matching products (in this case json files) of different entries that are found in several websites. This is also known as entity resolution or disambiguation in bibliography. To elaborate, we are given multiple folders which contain json files. Each folder represents an ecommerce website (E.g www.ebay.com) and each json file represent a selling product. We are also given a .csv file that contains mathcing products id's as a set of two. Below, we will explain how we match same products based on transitive property and also the structs that have been used throughout the programm.

### How to run
1. There is a makefile that responds to the following commands:

    * make -> compile the program and produce the corresponding object files and the executables, including unit test

    * make clean -> deletes the object files , executables and the output file that was created

2. Command for executing:
    ./find_commons -d path_to_directory -f file_with_matching_products -h hashtable_size 

    path_to_directory : path to directory which holds multiple subdirectories with json files
    file_with_matching_products : .csv file with matching products
    hashtable_size: size of hashtable 

### Testing
We have implemented a unit test and also used github continuous integration.

###  Basic data structures: 
* Red Black Tree
* Linked Lists
* Hashtable 

### Where products (aka json files) are stored ?
Each node of the red black tree represents a json file - product.

### Where are the categories and values of products stored ?
Each node in the red black tree, which represents a product, contains a list. Each node of the list represents a category and contains another list with the category's values.

### How we search a product ?
We have a hash table which is used for indexing and every position of the table points to a red black tree, that stores json instances with the same hash value. After hashing and traversing the red black tree we find the corresponding node that represents json file.

### How we represent same products ?
The tree node mentioned before, contains a pointer to a list in which we store the products he matches to. The list also contains the product itself. The list represents a clique.

### Algorithm of matching same products
All products that match point to the same list. When we find a new set of products that match we do as follows:
* We concatenate the lists of the two products and make all products point to the new list.
* For example: 
    list1 with same products -> (a, b). We can access it from tree nodes a and b which point to list1.
    list2 with same products -> (c, d, e). We can access it from tree nodes c, d and e which point to list2.
    * if b and c match we concatenate list1 and list2 and we make c, d, e tree nodes point to the new list. 
    * it is obvious that tree nodes a,b already point to the new list. 

### Why hash table and red black tree ?
* By using a red black tree we can search a node  in O(logn) time with n being the number of products. 
* By using a hash table with a good hash function we can reduce the complexity to O(1)*O(logn/M) with M being size of hash table.


### What's been added

* For every clique we find all products that are different.
* We split randomly our data
   * train_set: 80%
   * test_set: 20%
* Before training, we clean up the text:
   1. Lowercase
   2. Remove punctuation
   3. Remove numbers
* When calculating optimal weights we avoid unnecessary calculations, that gives a huge time boost to the procedure of training.
* Accuracy usually starts from ~77% and it increases for each epoch.

### Files
* RBtree.c, RBtree.h: Red black tree implementation
* HashTable.c, HashTable.h: Functions for hashtable 
* list.c, list.h: List with matching products
* jsonParser.c, jsonParser.h: Files for handling json files, including lists and a json parser
* helpFunctions.c, helpFunctions.h: Functions for reading input arguments and initializing data structures 
* unit_test.c: Unit test functions
* main.c: Main
* logistic_regression.c, logistic_regression.h: Logistic regression implementation
* dataList.c, dataList.h: lists for storing train and test data
* Makefile: Creates object files and executables, including unit_test
