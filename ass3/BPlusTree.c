#include "BPlusTree.h"
#include <stdbool.h>
#ifdef _WIN32
#define bool char
#define false 0
#define true 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

// Default order is 4.
#define DEFAULT_ORDER 4

// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
#define MIN_ORDER 3
#define MAX_ORDER 20

// Constant for optional command-line input with "i" command.
#define BUFFER_SIZE 256



// GLOBALS.
int order = DEFAULT_ORDER;
node *queue = NULL;
bool verbose_output = false;



void enqueue(node *new_node);
node *dequeue(void);
int height(node *const root);
int path_to_root(node *const root, node *child);
void print_leaves(node *const root);
void print_tree(node *const root);
void find_and_print(node *const root, char *key, int verbose);

node *find_leaf(node *const root, char *key, bool verbose);
// record * find(node *root, char *key, bool verbose, node ** leaf_out);
int cut(int length);


// Insertion.
record * make_record(unsigned int value);
node *make_node(void);
node *make_leaf(void);
int get_left_index(node *parent, node *left);
node *insert_into_leaf(node *leaf, char *key, record *pointer );
node *insert_into_leaf_after_splitting(node *root, node *leaf, char *key, record *pointer );
node *insert_into_node(node *root, node *parent, int left_index, char *key, node *right);
node *insert_into_node_after_splitting(node *root, node *parent, int left_index, char *key, node *right);
node *insert_into_parent(node *root, node *left, char *key, node *right);
node *insert_into_new_root(node *left, char *key, node *right);
node *start_new_tree( char *key, record *pointer );
node *insert( node *root, char *key, unsigned int offset );



void enqueue(node *new_node) {
	node *c;
	if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	}
	else {
		c = queue;
		while(c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		new_node->next = NULL;
	}
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
node *dequeue(void) {
	node *n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}


/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
void print_leaves(node *const root) {
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	int i;
	node *c = root;
	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			if (verbose_output)
				printf("%p ", c->pointers[i]);
			printf("%s ", c->keys[i]);
		}
		if (verbose_output)
			printf("%p ", c->pointers[order - 1]);
		if (c->pointers[order - 1] != NULL) {
			printf(" | ");
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height(node *const root) {
	int h = 0;
	node *c = root;
	while (!c->is_leaf) {
		c = c->pointers[0];
		h++;
	}
	return h;
}

/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root(node *const root, node *child) {
	int length = 0;
	node *c = child;
	while (c != root) {
		c = c->parent;
		length++;
	}
	return length;
}


/* Prints the B+ tree in the command
 * line in level (rank) order, with the
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
void print_tree(node *const root) {

	node *n = NULL;
	int i = 0;
	int rank = 0;
	int new_rank = 0;

	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	queue = NULL;
	enqueue(root);
	while(queue != NULL) {
		n = dequeue();
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			new_rank = path_to_root(root, n);
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		if (verbose_output)
			printf("(%p)", n);
		for (i = 0; i < n->num_keys; i++) {
			if (verbose_output)
				printf("%p ", n->pointers[i]);
			printf("%s ", n->keys[i]);
		}
		if (!n->is_leaf)
			for (i = 0; i <= n->num_keys; i++)
				enqueue(n->pointers[i]);
		if (verbose_output) {
			if (n->is_leaf)
				printf("%p ", n->pointers[order - 1]);
			else
				printf("%p ", n->pointers[n->num_keys]);
		}
		printf("| ");
	}
	printf("\n");

}


/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 */
void find_and_print(node *const root, char *key, int verbose) {
    // node *leaf = NULL;
	// record * r = find(root, key, verbose, NULL);
	// if (r == NULL)
	// 	printf("Record not found under key %s.\n", key);
	// else
	// 	printf("Record at %p -- key %s, value %d.\n",
	// 			r, key, r->value);
}

node *find_leaf(node *const root, char *key, bool verbose) {
	if (root == NULL) {
		if (verbose)
			printf("Empty tree.\n");
		return root;
	}
	int i = 0;
	node *c = root;
	while (!c->is_leaf) {
		if (verbose) {
			printf("[");
			for (i = 0; i < c->num_keys - 1; i++)
				printf("%s ", c->keys[i]);
			printf("%s] ", c->keys[i]);
		}
		i = 0;
		while (i < c->num_keys) {
			if ( strcmp ( key, c->keys[i] ) >= 0 ) i++;
			else break;
		}
		if (verbose)
			printf("%d ->\n", i);
		c = (node *)c->pointers[i];
	}
	if (verbose) {
		printf("Leaf [");
		for (i = 0; i < c->num_keys - 1; i++)
			printf("%s ", c->keys[i]);
		printf("%s] ->\n", c->keys[i]);
	}
	return c;
}

/* Finds and returns the record to which
 * a key refers.
 */
record *find(node *root, char *key, bool verbose, node ** leaf_out) {
    if (root == NULL) {
        if (leaf_out != NULL) {
            *leaf_out = NULL;
        }
        return NULL;
    }

	int i = 0;
    node *leaf = NULL;

	leaf = find_leaf(root, key, verbose);

    /* If root != NULL, leaf must have a value, even
     * if it does not contain the desired key.
     * (The leaf holds the range of keys that would
     * include the desired key.)
     */

	for (i = 0; i < leaf->num_keys; i++)
	   if ( strcmp( leaf->keys[i], key ) == 0 ) break;
    if (leaf_out != NULL) {
        *leaf_out = leaf;
    }
	if (i == leaf->num_keys)
		return NULL;
	else
		return (record *)leaf->pointers[i];
}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut(int length) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}


// INSERTION

/* Creates a new record to hold the value
 * to which a key refers.
 */
record * make_record(unsigned int value) {
	record * new_record = (record *)malloc(sizeof(record));
	if (new_record == NULL) {
		perror("Record creation.");
		exit(EXIT_FAILURE);
	}
	else {
		new_record->offset = value;
	}
	return new_record;
}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
node *make_node(void) {
	node *new_node;
	new_node = malloc(sizeof(node));
	if (new_node == NULL) {
		perror("Node creation.");
		exit(EXIT_FAILURE);
	}

	new_node->pointers = malloc(order * sizeof(void *));
	if (new_node->pointers == NULL) {
		perror("New node pointers array.");
		exit(EXIT_FAILURE);
	}
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;
	return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
node *make_leaf(void) {
	node *leaf = make_node();
	leaf->is_leaf = true;
	return leaf;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 */
int get_left_index(node *parent, node *left) {

	int left_index = 0;
	while (left_index <= parent->num_keys &&
			parent->pointers[left_index] != left)
		left_index++;
	return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
node *insert_into_leaf(node *leaf, char *key, record *pointer ) {

	int i, insertion_point;
	insertion_point = 0;
	while (insertion_point < leaf->num_keys && strcmp( leaf->keys[insertion_point], key ) < 0 )
		insertion_point++;


	for (i = leaf->num_keys; i > insertion_point; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}

	leaf->keys[insertion_point] = malloc(12);
	strcpy( leaf->keys[insertion_point], key );
	leaf->pointers[insertion_point] = pointer;
	leaf->num_keys++;
	return leaf;
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
node *insert_into_leaf_after_splitting(node *root, node *leaf, char *key, record *pointer) {

	node *new_leaf;
	char *temp_keys[DEFAULT_ORDER];
	void ** temp_pointers;
	int insertion_index, split, i, j;

	new_leaf = make_leaf();
	temp_pointers = malloc(order * sizeof(void *));
	if (temp_pointers == NULL) {
		perror("Temporary pointers array.");
		exit(EXIT_FAILURE);
	}

	insertion_index = 0;
	while (insertion_index < order - 1 && strcmp ( leaf->keys[insertion_index], key ) < 0 )
		insertion_index++;

	for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
		if (j == insertion_index) j++;

		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}

	temp_keys[insertion_index] = malloc(12);
	strcpy ( temp_keys[insertion_index], key );
	temp_pointers[insertion_index] = pointer;


	leaf->num_keys = 0;

	split = cut(order - 1);

	for (i = 0; i < split; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->num_keys++;
	}

	for (i = split, j = 0; i < order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->num_keys++;
	}

	free(temp_pointers);

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;

	for (i = leaf->num_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parent = leaf->parent;

	return insert_into_parent(root, leaf, new_leaf->keys[0], new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
node *insert_into_node(node *root, node *n, int left_index, char *key, node *right) {
	int i;
	for (i = n->num_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] =  n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
    n->keys[left_index] = malloc(12);
	strcpy( n->keys[left_index], key );
	n->num_keys++;
	return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node *insert_into_node_after_splitting(node *root, node *old_node, int left_index, char *key, node *right) {

	int i, j, split;
    char *k_prime;
	node *new_node, * child;
	char *temp_keys[DEFAULT_ORDER];
	node ** temp_pointers;

	/* First create a temporary set of keys and pointers
	 * to hold everything in order, including
	 * the new key and pointer, inserted in their
	 * correct places.
	 * Then create a new node and copy half of the
	 * keys and pointers to the old node and
	 * the other half to the new.
	 */

	temp_pointers = malloc((order + 1) * sizeof(node *));
	if (temp_pointers == NULL) {
		perror("Temporary pointers array for splitting nodes.");
		exit(EXIT_FAILURE);
	}

	for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
		if (j == left_index + 1) j++;
		temp_pointers[j] = old_node->pointers[i];
	}

	for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
		if (j == left_index) j++;
		temp_keys[j] = old_node->keys[i];
	}


    temp_keys[left_index] = malloc(12);
	strcpy( temp_keys[left_index], key );
    temp_pointers[left_index + 1] = right;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 */
	split = cut(order);
	new_node = make_node();
	old_node->num_keys = 0;

	for (i = 0; i < split - 1; i++) {
		old_node->pointers[i] = temp_pointers[i];
		old_node->keys[i] = temp_keys[i];
		old_node->num_keys++;
	}

	old_node->pointers[i] = temp_pointers[i];
	k_prime = temp_keys[split - 1];

    for (++i, j = 0; i < order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[j] = temp_pointers[i];
	free(temp_pointers);

	new_node->parent = old_node->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
		child = new_node->pointers[i];
		child->parent = new_node;
	}

	/* Insert a new key into the parent of the two
	 * nodes resulting from the split, with
	 * the old node to the left and the new to the right.
	 */

	return insert_into_parent(root, old_node, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node *insert_into_parent(node *root, node *left, char *key, node *right) {
	// printf("[keys]%s %s\n", root->keys[0], key);
	int left_index;
	node *parent;

	parent = left->parent;

	/* Case: new root. */

	if (parent == NULL) {
		return insert_into_new_root(left, key, right);
    }

	/* Case: leaf or node. (Remainder of
	 * function body.)
	 */

	/* Find the parent's pointer to the left
	 * node.
	 */

	left_index = get_left_index(parent, left);

	/* Simple case: the new key fits into the node.
	 */

	if (parent->num_keys < order - 1) {
		return insert_into_node(root, parent, left_index, key, right);
    }

	/* Harder case:  split a node in order
	 * to preserve the B+ tree properties.
	 */
	return insert_into_node_after_splitting(root, parent, left_index, key, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node *insert_into_new_root( node *left, char *key, node *right ) {

	node *root = make_node();
    root->keys[0] = malloc(12);
	strcpy( root->keys[0], key );
	root->pointers[0] = left;
	root->pointers[1] = right;
	root->num_keys++;
	root->parent = NULL;
	left->parent = root;
	right->parent = root;
	return root;
}



/* First insertion:
 * start a new tree.
 */
node *start_new_tree( char *key, record *pointer ) {

	node *root = make_leaf();
    root->keys[0] = malloc(12);
	strcpy( root->keys[0], key);
	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
	return root;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
node *insert(node *root, char *key, unsigned int offset ) {

	record *record_pointer = NULL;
	node *leaf = NULL;

	/* The current implementation ignores
	 * duplicates.
	 */

	record_pointer = find(root, key, false, NULL);
    if (record_pointer != NULL) {
        /* If the key already exists in this tree, update
         * the value and return the tree.
         */
         printf("key exists");
         return root;
    }

	/* Create a new record for the
	 * value.
	 */
	record_pointer = make_record( offset );


	/* Case: the tree does not exist yet.
	 * Start a new tree.
	 */

	if (root == NULL)
		return start_new_tree( key, record_pointer );


	/* Case: the tree already exists.
	 * (Rest of function body.)
	 */

	 leaf = find_leaf(root, key, false);

	/* Case: leaf has room for key and record_pointer.
	 */

	if (leaf->num_keys < order - 1) {
		leaf = insert_into_leaf(leaf, key, record_pointer );
		return root;
	}


	/* Case:  leaf must be split.
	 */
	return insert_into_leaf_after_splitting( root, leaf, key, record_pointer );
}


void destroy_tree_nodes(node *root) {
	int i;
	if (root->is_leaf)
		for (i = 0; i < root->num_keys; i++)
			free(root->pointers[i]);
	else
		for (i = 0; i < root->num_keys + 1; i++)
			destroy_tree_nodes(root->pointers[i]);
	free(root->pointers);
	free(root->keys);
	free(root);
}


node *destroy_tree(node *root) {
	destroy_tree_nodes(root);
	return NULL;
}
