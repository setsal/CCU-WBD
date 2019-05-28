#ifndef __BPlusTree_H__
#define __BPlusTree_H__

#define DEFAULT_ORDER 4
#define bool char

// TYPES.
typedef struct record {
	unsigned int offset;
} record;

typedef struct node {
	void ** pointers;
	unsigned char *keys[DEFAULT_ORDER-1];
	struct node *parent;
	bool is_leaf;
	int num_keys;
	struct node *next; // Used for queue.
	struct block *block[DEFAULT_ORDER-1];
} node;


extern void print_leaves(node *const);
extern void traversal_leaf_and_write_blockfile(node *const, char *, char *);
extern void traversal_leaf_and_append_block(node *const, unsigned int * );
extern void print_tree(node *const);
extern void find_and_print(node *const, unsigned char *, int, char *, char *);
extern node *insert( node *, unsigned char *, unsigned int );
extern node *insert_from_blockfile( node *, unsigned char *, unsigned int );
extern node *destroy_tree(node *);

#endif
