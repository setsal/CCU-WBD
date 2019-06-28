#include "BPlusTree.h"
#include "Block.h"
#define bool char
#define false 0
#define true 1
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
#define BLOCK_SIZE 64000
#define BASE_SIZE 17



// GLOBALS.
int order = DEFAULT_ORDER;
node *queue = NULL;
bool verbose_output = false;
const char *field[6] = { "\turl:https://www.youtube.com/watch?v=", "\ttitle:", "\tcontent:", "\tviewCount:", "\tres:", "\tduration:" };
unsigned int pDbFileMapOffset, pDbFileOffset;
unsigned char blockbuffer[BLOCK_SIZE];

// BLOCK GLOBALS
unsigned char base[BASE_SIZE];
unsigned char baseKey[4];

//temp used
Block* tmpBlock;
unsigned char *tmpptr;

// BLOCK function
void insert_into_block( Block *TargetBlock, unsigned char base[] );
Block *start_new_block();
void buildBase( unsigned char *rid );
void IntToByte( unsigned int value );

void enqueue(node *new_node);
node *dequeue(void);
int height(node *const root);
int path_to_root(node *const root, node *child);
void print_leaves(node *const root);
void print_tree(node *const root);
void find_and_print(node *const root, unsigned char *key, int verbose, char *l_dbfile, char *l_dbfilemap);
void find_and_print2(node *const root, unsigned char *key, int verbose, char *l_dbfile, char *l_dbfilemap, FILE * writefp);
node *find_leaf(node *const root, unsigned char *key, bool verbose);
record *find(node *root, unsigned char *key, bool verbose, node ** leaf_out);
record *find_close(node *root, unsigned char *key, bool verbose, node ** leaf_out);

int cut(int length);


// Insertion.
record * make_record(unsigned int value);
node *make_node(void);
node *make_leaf(void);
int get_left_index(node *parent, node *left);
node *insert_into_leaf(node *leaf, unsigned char *key, record *pointer );
node *insert_into_leaf_after_splitting(node *root, node *leaf, unsigned char *key, record *pointer );
node *insert_into_node(node *root, node *parent, int left_index, unsigned char *key, node *right);
node *insert_into_node_after_splitting(node *root, node *parent, int left_index, unsigned char *key, node *right);
node *insert_into_parent(node *root, node *left, unsigned char *key, node *right);
node *insert_into_new_root(node *left, unsigned char *key, node *right);
node *start_new_tree( unsigned char *key, record *pointer );
node *insert( node *root, unsigned char *key, unsigned int offset );


//print
void traversal_leaf_and_write_blockfile(node *const root, char *l_blockfile, char *l_blockfile_info);

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


node *dequeue(void) {
	node *n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}


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
			printf("%s ", c->keys[i]);
		}
		if (c->pointers[order - 1] != NULL) {
			printf(" | ");
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	printf("\n");
}



void traversal_leaf_and_write_blockfile(node *const root, char *l_blockfile, char *l_blockfile_info ) {
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	int i, j;
	node *c = root;

	FILE *pblockfile, *pblockfileInfo;
	pblockfile = fopen( l_blockfile, "wb+");
	pblockfileInfo = fopen( l_blockfile_info, "w+");

	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			// printf("[INFO] Write block in key %s %d %u\n", c->keys[i], ((record *)c->pointers[i])->offset, c->block[i]->blockSize );
			// printf("num key:%d\n", c->num_keys);
			// Write the block information
			fprintf( pblockfileInfo, "%s %d %u\n", c->block[i]->blockptr, ((record *)c->pointers[i])->offset, c->block[i]->blockSize );

			// // Write the block into blockfile
			fwrite(c->block[i]->blockptr, c->block[i]->blockSize, 1, pblockfile);
			for ( j=c->block[i]->blockSize; j<BLOCK_SIZE; j++) {
				fputc(0, pblockfile);
			}

		}
		if (c->pointers[order - 1] != NULL) {
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	fclose(pblockfile);
	fclose(pblockfileInfo);
	printf("\n");
}


void traversal_leaf_and_append_block(node *const root, unsigned int offset_array[], char *l_blockfile ) {
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	int i, j, nblock = 0;
	node *c = root;

	FILE *fp;
	fp = fopen( l_blockfile, "rb+");

	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			c->block[i] = start_new_block();
			c->block[i]->blockSize = offset_array[nblock];
			fread( c->block[i]->blockptr, BLOCK_SIZE, 1, fp );
			// printf("1:%s \n", c->keys[i]);
			// printf("2:%s \n", c->block[i]->blockptr);
			nblock++;
		}
		if (c->pointers[order - 1] != NULL) {
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	fclose(fp);
	printf("\n");
}


int height(node *const root) {
	int h = 0;
	node *c = root;
	while (!c->is_leaf) {
		c = c->pointers[0];
		h++;
	}
	return h;
}


int path_to_root(node *const root, node *child) {
	int length = 0;
	node *c = child;
	while (c != root) {
		c = c->parent;
		length++;
	}
	return length;
}


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


void find_and_print(node *const root, unsigned char *key, int verbose, char *l_dbfile, char *l_dbfilemap ) {
    node *leaf = NULL;
	FILE *fp;
	record * r = find(root, key, verbose, NULL);
	unsigned char s[20];
	char str[500];
	int i, offset = 0, isFind = 1;

	FILE *writefp = fopen("tmp.txt", "w");
	FILE *infofp = fopen("info.txt", "w");

	if (r == NULL) {
		fprintf( infofp, "[INFO] Record not found under key %s. Trying search in the closet block.\n", key);
		// record * r = find_close(root, key, verbose, NULL);

		leaf = find_leaf(root, key, verbose);
		for (i = 0; i < leaf->num_keys; i++)
		   if ( strcmp( leaf->keys[i], key ) >= 0 ) break;

		i = i -1;
		fprintf( infofp, "[INFO] In the closet block: %s\n", leaf->keys[i]);

		// Do sequence search in the block
		unsigned char *blockptr = leaf->block[i]->blockptr;
		while ( strcmp(blockptr, key) != 0 ) {
			if( offset+17 > leaf->block[i]->blockSize ) {
				isFind = 0;
				break;
			}
			blockptr+=17;
			offset+=17;
		}


		if ( isFind == 0 ) {
			fprintf( infofp, "[Exception] Still not found. Check your key value is exist in origin data or not?\n");
			return;
		}

		blockptr+=12;
		for ( i=0; i<4; i++){
			s[i] = *blockptr++;
		}

		// Get db file Map offset
		pDbFileMapOffset = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		fprintf( infofp, "[SUCCESS] Record find -- key %s, pDbFileMapOffset %u, ",  key, pDbFileMapOffset);

		// Get db file Map Value
		fp = fopen( l_dbfilemap,"r");
		fseek(fp, pDbFileMapOffset, SEEK_SET);
		fscanf(fp, "%u", &pDbFileOffset);
		fprintf( infofp, "pDbFileOffset %u.\n", pDbFileOffset);
		fclose(fp);

		// Get db file Value
		fp = fopen( l_dbfile,"r");
		fseek(fp, pDbFileOffset, SEEK_SET);
		fgets( str, 500, fp );
		fclose(fp);

		// Print the youtube data detail
		char *tmp = str;
		int counter = 0;
		fprintf( writefp, "%s", field[0]);
		for ( i=0; i<strlen(str); i++ ) {
			if ( str[i] == '\t' ) {
				counter++;
				fprintf( writefp, "\n%s", field[counter]);
			}
			else {
				fprintf( writefp, "%c", str[i]);
			}
		}

	}
	else {
		fprintf( infofp, "Record at %p -- key %s, value %d\n", r, key, r->offset);

		// Get db file Map Value
		fp = fopen( l_dbfilemap,"r");
		fseek(fp, r->offset, SEEK_SET);
		fscanf(fp, "%u", &pDbFileOffset);
		fprintf( infofp, "pDbFileOffset %u.\n", pDbFileOffset);
		fclose(fp);
		//
		// // Get db file Value
		fp = fopen( l_dbfile,"r");
		fseek(fp, pDbFileOffset, SEEK_SET);
		fgets( str, 500, fp );
		fclose(fp);

		// // Print the youtube data detail
		char *tmp = str;
		int counter = 0;
		fprintf( writefp, "%s", field[0]);
		for ( i=0; i<strlen(str); i++ ) {
			if ( str[i] == '\t' ) {
				counter++;
				fprintf( writefp, "\n%s", field[counter]);
			}
			else {
				fprintf( writefp, "%c", str[i]);
			}
		}

	}
	fclose(infofp);
	fclose(writefp);
	printf("[INFO] Success find and write\n");

}


node *find_leaf(node *const root, unsigned char *key, bool verbose) {
	if (root == NULL) {
		return root;
	}
	int i = 0;
	node *c = root;
	while (!c->is_leaf) {
		i = 0;
		while (i < c->num_keys) {
			if ( strcmp ( key, c->keys[i] ) >= 0 ) i++;
			else break;
		}
		c = (node *)c->pointers[i];
	}
	return c;
}


record *find(node *root, unsigned char *key, bool verbose, node ** leaf_out) {
    if (root == NULL) {
        if (leaf_out != NULL) {
            *leaf_out = NULL;
        }
        return NULL;
    }

	int i = 0;
    node *leaf = NULL;

	leaf = find_leaf(root, key, verbose);


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


record *find_close(node *root, unsigned char *key, bool verbose, node ** leaf_out) {
    if (root == NULL) {
        if (leaf_out != NULL) {
            *leaf_out = NULL;
        }
        return NULL;
    }

	int i = 0;
    node *leaf = NULL;

	leaf = find_leaf(root, key, verbose);

	for (i = 0; i < leaf->num_keys; i++)
	   if ( strcmp( leaf->keys[i], key ) <= 0 ) break;
    if (leaf_out != NULL) {
        *leaf_out = leaf;
    }
	if (i == leaf->num_keys)
		return NULL;
	else
		return (record *)leaf->pointers[i];
}


int cut(int length) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}


// INSERTION
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


	new_node->block[0] = malloc(sizeof(Block));
	new_node->block[0]->blockSize = 0;
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;
	return new_node;
}

node *make_new_root_node(void) {

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


node *make_leaf(void) {
	node *leaf = make_node();
	leaf->is_leaf = true;
	return leaf;
}


int get_left_index(node *parent, node *left) {

	int left_index = 0;
	while (left_index <= parent->num_keys &&
			parent->pointers[left_index] != left)
		left_index++;
	return left_index;
}


node *insert_into_leaf(node *leaf, unsigned char *key, record *pointer ) {

	int i, insertion_point = 0;


	// find block insert point
	for ( i=0; i<leaf->num_keys; i++ ) {
		if ( strcmp( leaf->block[i]->blockptr, key ) < 0 ) {
			insertion_point = i;
		}
		else {
			break;
		}
	}

	if ( leaf->block[insertion_point]->blockSize + 17 < BLOCK_SIZE ) {
		// printf("Insert into block %d - %s, %u\n", insertion_point, key, pointer->offset );
		IntToByte( pointer->offset );
		buildBase( key );
		insert_into_block( leaf->block[insertion_point], base );

		// Change Now Key Value
		strcpy ( leaf->keys[insertion_point], leaf->block[insertion_point]->blockptr );
		return leaf;
	}
	else {
		printf("NEED SPLIT\n");
		//split and add to b plus tree
		tmpBlock = split_block( leaf->block[insertion_point], base );


		//Change the new key values
		tmpptr = tmpBlock->blockptr;
		tmpptr+=12;
		for ( i=0; i<4; i++){
			baseKey[i] = *tmpptr++;
		}

		// Get New offset
		pDbFileMapOffset = baseKey[0] << 24 | baseKey[1] << 16 | baseKey[2] << 8 | baseKey[3];
		record *new_pointer = make_record(pDbFileMapOffset);

		// Renew insert point to zero for B plus tree
		insertion_point = 0;
		while (insertion_point < leaf->num_keys && strcmp( leaf->keys[insertion_point], tmpBlock->blockptr ) < 0 )
			insertion_point++;


		// printf("Insert new split node to blus tree node %d\n", insertion_point);

		for (i = leaf->num_keys; i > insertion_point; i--) {
			leaf->keys[i] = leaf->keys[i - 1];
			leaf->block[i] = leaf->block[i - 1];
			leaf->pointers[i] = leaf->pointers[i - 1];
		}

		leaf->keys[insertion_point] = malloc(12);
		leaf->block[insertion_point] = tmpBlock;
		strcpy( leaf->keys[insertion_point], tmpBlock->blockptr );
		leaf->pointers[insertion_point] = new_pointer;
		leaf->num_keys++;


		// split done, then insert again
		insertion_point = 0;
		for ( i=0; i<leaf->num_keys; i++ ) {
			if ( strcmp( leaf->block[i]->blockptr, key ) < 0 ) {
				insertion_point = i;
			}
			else {
				break;
			}
		}
		// printf("Insert into block %d, %s, %u\n", insertion_point, key, pointer->offset );
		IntToByte( pointer->offset );
		buildBase( key );
		insert_into_block( leaf->block[insertion_point], base );
		strcpy ( leaf->keys[insertion_point], leaf->block[insertion_point]->blockptr );

		return leaf;
	}

}


node *insert_into_leaf_blockfile(node *leaf, unsigned char *key, unsigned int offset ) {

	int i, insertion_point = 0;

	record *new_pointer = make_record(offset);


	insertion_point = 0;
	while (insertion_point < leaf->num_keys && strcmp( leaf->keys[insertion_point], key ) < 0 )
		insertion_point++;

	for (i = leaf->num_keys; i > insertion_point; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}

	leaf->keys[insertion_point] = malloc(12);
	strcpy( leaf->keys[insertion_point], key );
	leaf->pointers[insertion_point] = new_pointer;
	leaf->num_keys++;

	return leaf;
}


node *splitting_leaf( node *root, node *leaf ) {
	node *new_leaf;
	int split, i, j;

	new_leaf = make_leaf();

	split = cut(order - 1);


	for (i = split, j = 0; i < leaf->num_keys; i++, j++) {
		new_leaf->pointers[j] = leaf->pointers[i];
		new_leaf->keys[j]= malloc(12);
		strcpy ( new_leaf->keys[j] , leaf->keys[i] );
		new_leaf->block[j] = leaf->block[i];
		new_leaf->num_keys++;
	}

	leaf->num_keys = split;

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;


	for (i = leaf->num_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parent = leaf->parent;



	return insert_into_parent(root, leaf, new_leaf->keys[0], new_leaf);
}


node *splitting_leaf_blockfile( node *root, node *leaf ) {
	node *new_leaf;
	int split, i, j;

	new_leaf = make_leaf();

	split = cut(order - 1);


	for (i = split, j = 0; i < leaf->num_keys; i++, j++) {
		new_leaf->pointers[j] = leaf->pointers[i];
		new_leaf->keys[j]= malloc(12);
		strcpy ( new_leaf->keys[j] , leaf->keys[i] );
		new_leaf->num_keys++;
	}

	leaf->num_keys = split;

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;


	for (i = leaf->num_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parent = leaf->parent;


	return insert_into_parent(root, leaf, new_leaf->keys[0], new_leaf);
}


node *insert_into_leaf_after_splitting(node *root, node *leaf, unsigned char *key, record *pointer) {

	node *new_leaf;
	char *temp_keys[DEFAULT_ORDER];
	// block *temp_block[DEFAULT_ORDER];
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


node *insert_into_node(node *root, node *n, int left_index, unsigned char *key, node *right) {
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


node *insert_into_node_after_splitting(node *root, node *old_node, int left_index, unsigned char *key, node *right) {

	int i, j, split;
    unsigned char *k_prime;
	node *new_node, * child;
	unsigned char *temp_keys[DEFAULT_ORDER];
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


node *insert_into_parent(node *root, node *left, unsigned char *key, node *right) {
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


node *insert_into_new_root( node *left, unsigned char *key, node *right ) {

	node *root = make_new_root_node();
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


node *start_new_tree( unsigned char *key, record *pointer ) {

	node *root = make_leaf();

    root->keys[0] = malloc(12);
	root->block[0]->blockptr = malloc(BLOCK_SIZE);
	strcpy( root->keys[0], key);

	// block insert
	IntToByte( pointer->offset );
	buildBase( key );
	insert_into_block( root->block[0], base );

	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
	return root;
}


node *start_new_tree_blockfile( unsigned char *key, record *pointer ) {
	node *root = make_leaf();
    root->keys[0] = malloc(12);
	strcpy( root->keys[0], key);
	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
	return root;
}


node *insert(node *root, unsigned char *key, unsigned int offset ) {

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
		if ( leaf->num_keys == order - 1 ) {
			// Do Split Tree
			return splitting_leaf( root, leaf );
		}
		return root;
	}


	/* Case:  leaf must be split.
	 */
	 printf("IS HERE?");
	return insert_into_leaf_after_splitting( root, leaf, key, record_pointer );
}


node *insert_from_blockfile(node *root, unsigned char *key, unsigned int offset ) {


	record *record_pointer = NULL;
	node *leaf = NULL;

	record_pointer = find(root, key, false, NULL);
    if (record_pointer != NULL) {
         printf("key exists");
         return root;
    }

	record_pointer = make_record( offset );

	if (root == NULL)
		return start_new_tree_blockfile( key, record_pointer );

	 leaf = find_leaf(root, key, false);

	if (leaf->num_keys < order - 1) {
		leaf = insert_into_leaf_blockfile(leaf, key, offset );
		if ( leaf->num_keys == order - 1 ) {
			return splitting_leaf_blockfile( root, leaf );
		}
		return root;
	}

	 printf("IS HERE?");
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


void IntToByte( unsigned int value ) {
    baseKey[0] = (int)((value >> 24) & 0xFF);
    baseKey[1] = (int)((value >> 16) & 0xFF);
    baseKey[2] = (int)((value >> 8) & 0xFF);
    baseKey[3] = (int)(value & 0xFF);
}


void buildBase( unsigned char *rid ) {
    int i = 0;

    //record id
    for( i=0; i<11; i++ ) {
        base[i] = *(rid+i);
    }
    base[11] = '\0';

    //key value
    base[12] = baseKey[0];
    base[13] = baseKey[1];
    base[14] = baseKey[2];
    base[15] = baseKey[3];
    base[16] = '\0';
}


node *destroy_tree(node *root) {
	destroy_tree_nodes(root);
	return NULL;
}


void find_and_print2(node *const root, unsigned char *key, int verbose, char *l_dbfile, char *l_dbfilemap, FILE * writefp ) {
    node *leaf = NULL;
	FILE *fp;
	record * r = find(root, key, verbose, NULL);
	unsigned char s[20];
	char str[500];
	int i, offset = 0, isFind = 1;

	if (r == NULL) {

		leaf = find_leaf(root, key, verbose);
		for (i = 0; i < leaf->num_keys; i++)
		   if ( strcmp( leaf->keys[i], key ) >= 0 ) break;

		i = i -1;

		// Do sequence search in the block
		unsigned char *blockptr = leaf->block[i]->blockptr;
		while ( strcmp(blockptr, key) != 0 ) {
			if( offset+17 > leaf->block[i]->blockSize ) {
				isFind = 0;
				break;
			}
			blockptr+=17;
			offset+=17;
		}


		if ( isFind == 0 ) {
			return;
		}

		blockptr+=12;
		for ( i=0; i<4; i++){
			s[i] = *blockptr++;
		}

		// Get db file Map offset
		pDbFileMapOffset = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];

		// Get db file Map Value
		fp = fopen( l_dbfilemap,"r");
		fseek(fp, pDbFileMapOffset, SEEK_SET);
		fscanf(fp, "%u", &pDbFileOffset);
		fclose(fp);

		// Get db file Value
		fp = fopen( l_dbfile,"r");
		fseek(fp, pDbFileOffset, SEEK_SET);
		fgets( str, 500, fp );
		fclose(fp);

		// Print the youtube data detail
		char *tmp = str;
		int counter = 0;
		fprintf( writefp, "%s", field[0]);
		for ( i=0; i<strlen(str); i++ ) {
			if ( str[i] == '\t' ) {
				counter++;
				fprintf( writefp, "\n%s", field[counter]);
			}
			else {
				fprintf( writefp, "%c", str[i]);
			}
		}

	}
	else {

		// Get db file Map Value
		fp = fopen( l_dbfilemap,"r");
		fseek(fp, r->offset, SEEK_SET);
		fscanf(fp, "%u", &pDbFileOffset);
		fclose(fp);
		//
		// // Get db file Value
		fp = fopen( l_dbfile,"r");
		fseek(fp, pDbFileOffset, SEEK_SET);
		fgets( str, 500, fp );
		fclose(fp);

		// // Print the youtube data detail
		char *tmp = str;
		int counter = 0;
		fprintf( writefp, "%s", field[0]);
		for ( i=0; i<strlen(str); i++ ) {
			if ( str[i] == '\t' ) {
				counter++;
				fprintf( writefp, "\n%s", field[counter]);
			}
			else {
				fprintf( writefp, "%c", str[i]);
			}
		}

	}
	printf("[INFO] Success find and write\n");
}
