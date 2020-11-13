/*
 * Author : Krishang Patney 
 * StudentID : 2405690p
 * File : tldlist.c
 * Assignment Title : SP Exerscise 1A
 * This is my own work as defined in the Academic Ethics agreement I have signed.
 */

#include "tldlist.h"
#include "date.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// test lines for tldmonitor
// ./tldmonitor 01/01/2017 01/09/2020 <small.txt | sort -n | diff - small.out
// ./tldmonitor 01/01/2017 01/09/2020 <large.txt | sort -n | diff - large.out

// Declerations for herlper functions
static TLDNode *tld_add_node(TLDList *tld, TLDNode *node, char *node_name);
static TLDNode *create_node(char *node_name);
static void tldlist_iter_add(TLDIterator *iter, TLDNode *node, int *iterator);

static int get_balance(TLDNode *node);
static int get_height(TLDNode *node);

static TLDNode *rotate_right_then_right(TLDNode *node);
static TLDNode *rotate_right_then_left(TLDNode *node);
static TLDNode *rotate_left_then_right(TLDNode *node);
static TLDNode *rotate_left_then_left(TLDNode *node);
static TLDNode *balance(TLDNode *node);


// Structure for tldlist 
struct tldlist{
    TLDNode *root;
     
    long count;
    long size;

    Date *begin;
    Date *end;
};

// Structure for tldnode
struct tldnode{
    char *node_name;
    long count;

    TLDNode *left;
    TLDNode *right;

};

// Structure for tlditerator
struct tlditerator{
    struct tldlist *iter;

    long size;
    int iteration;
    TLDNode **double_node;
};

/*
 * tldlist_create generates a list structure for storing counts against
 * top level domains (TLDs)
 *
 * creates a TLDList that is constrained to the `begin' and `end' Date's
 * returns a pointer to the list if successful, NULL if not
 */
TLDList *tldlist_create(Date *begin, Date *end){
    TLDList *tld_list = malloc(sizeof(TLDList));

    if(tld_list != NULL){
        tld_list -> root = NULL;

        tld_list -> count = 0;
        tld_list -> size = 0;

        tld_list -> begin = begin;
        tld_list -> end = end;
    }
    
    return tld_list;
}

/*
 * tldlist_destroy destroys the list structure in `tld'
 *
 * all heap allocated storage associated with the list is returned to the heap
 */
void tldlist_destroy(TLDList *tld){
    // if the tld is not null, free it.
    if (tld != NULL){
        free(tld);
    }
}

/*
 * tldlist_add adds the TLD contained in `hostname' to the tldlist if
 * `d' falls in the begin and end dates associated with the list;
 * returns 1 if the entry was counted, 0 if not
 */
int tldlist_add(TLDList *tld, char *hostname, Date *d){

    // Compare dates(d) to see if its under the date we want 
    // if date is less than day/month and more than day/month
    if (!(date_compare(tld -> begin, d) > 0 || date_compare(tld -> end, d) < 0)){
         // Gets the last word in string after "." chr
        char *tld_name = strrchr(hostname, '.') + 1;
        // Since the spec sheet says it needs to be lowercase this makes sure all the chars are lowercase
        for(int i = 0; i < strlen(tld_name); i++)
            tld_name[i] = tolower(tld_name[i]);

        tld_name = strdup(tld_name);
        // add root node here for said list.
        tld -> root  = tld_add_node(tld, tld -> root, tld_name);
        tld -> count++;
        
        return 1;
    };
    
   
    return 0;
}

/*
 * tldlist_count returns the number of successful tldlist_add() calls since
 * the creation of the TLDList
 */
long tldlist_count(TLDList *tld){
    return tld->count;
}

/*
 * tldlist_iter_create creates an iterator over the TLDList; returns a pointer
 * to the iterator if successful, NULL if not
 */
TLDIterator *tldlist_iter_create(TLDList *tld){
    TLDIterator *tldlist_iter = (TLDIterator *) malloc(sizeof(TLDIterator));

    if (tldlist_iter == NULL){
        free(tldlist_iter);
        return NULL;
    } else {
        tldlist_iter -> iter = tld; 
        tldlist_iter -> size = tld -> size;
        tldlist_iter -> iteration = 0;
        TLDNode **next_node = (TLDNode **)malloc(tldlist_iter -> size * sizeof(TLDNode *) );
        tldlist_iter -> double_node = next_node;
        if (tldlist_iter -> double_node == NULL){
            tldlist_iter_destroy(tldlist_iter);
            return NULL;
        }
    
        int iteration = 0;
        tldlist_iter_add(tldlist_iter, tldlist_iter->iter->root, &iteration);

        return tldlist_iter;
    

    }

}

/*
 * tldlist_iter_next returns the next element in the list; returns a pointer
 * to the TLDNode if successful, NULL if no more elements to return
 */
TLDNode *tldlist_iter_next(TLDIterator *iter){
    if (iter -> iteration == iter -> size){
        return NULL; //Iteration has reached the end of said tree
    }

    return *(iter -> double_node + iter -> iteration++);
}

/*
 * tldlist_iter_destroy destroys the iterator specified by `iter'
 */
void tldlist_iter_destroy(TLDIterator *iter){
    int i = 0;
    while (i < (iter->size))
    {   
        free(iter->double_node[i]->node_name);
        free(iter->double_node[i]);
        i++;	
    }
    free(iter->double_node);
    free(iter);
}

/*
 * tldnode_tldname returns the tld associated with the TLDNode
 */
char *tldnode_tldname(TLDNode *node){
    if (node -> node_name == NULL){
        return NULL;
    }
    return node -> node_name;
}

/*
 * tldnode_count returns the number of times that a log entry for the
 * corresponding tld was added to the list
 */
long tldnode_count(TLDNode *node){
    return node -> count;
}

/*
 * tld_add_node is an helper function for adding nodes to tldlist.
 */
static TLDNode *tld_add_node(TLDList *tld, TLDNode *node, char *node_name){
    // Insert and create a new node if it doesnt exist 
    if (node == NULL){
        node = create_node(node_name);
        tld -> root = node;
        tld -> size++;
        return node;
    }
    // Add to right after comparision
    else if (strcmp(node_name,  node-> node_name) > 0)
    {
        node -> right = tld_add_node(tld, node->right, node_name);
        // Balance Tree
        node = balance(node);
    } else if (strcmp(node_name, node->node_name) < 0){
        node -> left = tld_add_node(tld, node->left, node_name);
        // Balance Tree 
        node = balance(node);
    } else {
        free(node_name);
        node -> count++;
    }
    return node;
}
/*
 * create_node is an helper function for creating a node.
 */
static TLDNode *create_node(char *node_name){
    TLDNode *node = malloc(sizeof(TLDNode));
    if (node == NULL){
        free(node);
        return NULL;
    }
    else{
        node -> node_name = node_name;
        node -> left = NULL;
        node -> right = NULL;
        node -> count = 1;

        return node;

    }
   
}

/*
 * Helper function for tldlist_iter_create, which recursively 
 * adds a iter to its position
 */
static void tldlist_iter_add(TLDIterator *iter, TLDNode *node, int *iterator){
    // Traverse to the left
    if(node -> left){
        tldlist_iter_add(iter, node -> left, iterator);
    }
    
    *(iter -> double_node + (*iterator)++) = node;
    
    // Traverse to the right
    if (node -> right){
        tldlist_iter_add(iter, node -> right, iterator);
    }
}


/*
 * Helper function for to get tree balance
 */
static int get_balance(TLDNode *node){
    if (node == NULL){
        return 0;
    }
    return get_height(node -> left) - get_height(node -> right);
}
/*
 * Helper function to get tree height.
 */
static int get_height(TLDNode *node){
    int height = 0;
    int left_height;
    int right_height;
    int tallest_branch;

    if (node != NULL){
        left_height = get_height(node->left);
        right_height = get_height(node->right);

        if (left_height>right_height){
            tallest_branch = left_height;
        }else{
            tallest_branch = right_height;
        }
        height = tallest_branch + 1;
    }
    return height;
}


// Combined Rotations - rl,rr,lr,ll

/*
 * rotational function 
 */
static TLDNode *rotate_right_then_right(TLDNode *node){
    TLDNode *temp_node;
    temp_node = node -> right;
    node -> right = temp_node -> left;
    temp_node -> left = node;
    return temp_node;
}

/*
 * rotational function 
 */
static TLDNode *rotate_right_then_left(TLDNode *node){
    TLDNode *temp_node;
    temp_node = node -> right;
    node -> right = rotate_left_then_left(temp_node);
    return rotate_right_then_right(node);
    
}  

/*
 * rotational function 
 */
static TLDNode *rotate_left_then_left(TLDNode *node){
    TLDNode *temp_node;
    temp_node = node -> left;
    node -> left = temp_node -> right;
    temp_node -> right = node;
    return temp_node;
}

/*
 * rotational function 
 */
static TLDNode *rotate_left_then_right(TLDNode *node){
    TLDNode * temp_node;
    temp_node = node -> left;
    node -> left = rotate_right_then_right(temp_node);
    return rotate_left_then_left(node);
}

/*
 * Balances function for AVL trees
 */
static TLDNode *balance(TLDNode *node){
    int bal = get_balance(node);

    if (bal > 1){
        if (get_balance(node -> left) > 0){
            node = rotate_left_then_left(node);
        } else {
            node = rotate_left_then_right(node);
        }
    } else if (bal < -1){
        if (get_balance(node -> right) > 0){
            node = rotate_right_then_left(node);
        } else {
            node = rotate_right_then_right(node);
        }
    }
    return node;
}

