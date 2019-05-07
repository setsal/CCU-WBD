#include "BPlusTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

struct BPlusTreeNode* Root;

int MaxChildNumber = 3;
int TotalNodes;

int QueryAnsNum;

/** Create a new B+tree Node */
BPlusTreeNode* New_BPlusTreeNode() {
	struct BPlusTreeNode* p = (struct BPlusTreeNode*)malloc(sizeof(struct BPlusTreeNode));
	p->isRoot = false;
	p->isLeaf = false;
	p->key_num = 0;
	p->child[0] = NULL;
	p->father = NULL;
	p->next = NULL;
	p->last = NULL;
	TotalNodes++;
	return p;
}

/** Binary search to find the biggest child l that Cur->key[l] <= key */
int Binary_Search(BPlusTreeNode *Cur, char *key) {
	int start = 0, end = Cur->key_num;

	if ( Cur->key_num == 0 ) {
		return -1;
	}

	if ( strcmp ( key, Cur->key[start] ) < 0 ) {
		return start;
	}

	if ( strcmp ( key, Cur->key[end-1] ) >= 0 ) {
		return end-1;
	}

	while ( start < end-1 ) {

		// get the middle
		int mid = (start + end) >> 1;

		//to left leaf or right leaf
		if ( strcmp( Cur->key[mid], key ) > 0 )
			end = mid;
		else
			start = mid;
	}
	return start;
}

/**
 * Cur(MaxChildNumber) split into two part:
 *	(1) Cur(0 .. Mid - 1) with original key
 *	(2) Temp(Mid .. MaxChildNumber) with key[Mid]
 *  (3) Mid = MaxChildNumber / 2
 */
void Insert(BPlusTreeNode*, char *, BPlusTreeNode*);
void Split(BPlusTreeNode* Cur) {

	// copy Cur(Mid .. MaxChildNumber) -> Temp(0 .. Temp->key_num)
	BPlusTreeNode* Temp = New_BPlusTreeNode();
	BPlusTreeNode* ch;

	// Mid = MaxChildNumber/2
	int Mid = MaxChildNumber >> 1;

	// Split Half of Cur to Temp
	Temp->isLeaf = Cur->isLeaf;
	Temp->key_num = MaxChildNumber - Mid;


	int i;
	printf("mid: %d\n", Mid);
	for( i=Mid; i<MaxChildNumber; i++) {
		Temp->child[i-Mid] = Cur->child[i];
		Temp->key[i-Mid] = Cur->key[i];

		if( Temp->isLeaf ) {
			printf("is leaf!");
			// Temp->pos[i-Mid] = Cur->pos[i];
		} else {
			printf("loopinner");
			ch = Temp->child[i-Mid];
			ch->father = Temp;
			printf("loopinner ned");
		}

	}

	// Change Cur
	Cur->key_num = Mid;

	// Create new Root, increased the tree depth
	if( Cur->isRoot ) {
		Root = New_BPlusTreeNode();
		Root->key_num = 2;
		Root->isRoot = true;

		Root->key[0] = Cur->key[0];
		Root->child[0] = Cur;

		Root->key[1] = Temp->key[0];
		Root->child[1] = Temp;

		Cur->father = Temp->father = Root;
		Cur->isRoot = false;

		if ( Cur->isLeaf ) {
			Cur->next = Temp;
			Temp->last = Cur;
		}

	} else {
		// Try to insert Temp to Cur->father
		printf("Cur->key[Mid] : %s\n", Cur->key[Mid] );
		Temp->father = Cur->father;
		Insert( Cur->father, Cur->key[Mid], Temp );
	}

	printf("----SPLIT-----\n");
	printf("Block 1:");
	for ( i=0; i<Root->child[0]->key_num; i++ ) {
		printf(" %s,", Root->child[0]->key[i] );
	}
	printf("\n");
	printf("Block 2:");
	for ( i=0; i<Root->child[1]->key_num; i++ ) {
		printf(" %s,", Root->child[1]->key[i] );
	}
	printf("\n");
	printf("----SPLIT-----\n");
}

/** Insert (key, value) into Cur, if Cur is full, then split it to fit the definition of B+tree */
void Insert( BPlusTreeNode *Cur, char *key, BPlusTreeNode *New) {

	int i, ins;

	if ( Cur->key_num == 0 ) {
		ins = 0;
	}
	else if ( strcmp( key, Cur->key[0] ) < 0 ) {
		ins = 0;
	}
	else {
		ins = Binary_Search(Cur, key) + 1;
	}
	printf("Ins:%d, Cur->keynum: %d\n", ins, Cur->key_num);
	for (i = Cur->key_num; i > ins; i--) {
		Cur->key[i] = Cur->key[i - 1];
		if (Cur->isLeaf) {
			// Cur->child[i] = Cur->child[i-1];
			//Cur->pos[i] = Cur->pos[i - 1];
		}
	}
	Cur->key_num++;
	Cur->key[ins] = malloc(12);
	strcpy( Cur->key[ins], key );


	// make links on leaves, if root or upper change
	if ( Cur->isLeaf == false ) {
		BPlusTreeNode *firstChild = Cur->child[0];

		// which means value is also a leaf as child[0]
		if (firstChild->isLeaf == true) {
			if (ins > 0) {
				BPlusTreeNode *prevChild;
				BPlusTreeNode *succChild;
				prevChild = Cur->child[ins-1];
				succChild = prevChild->next;
				prevChild->next = New;
				New->next = succChild;
				New->last = prevChild;
				if (succChild != NULL) {
					succChild->last = New;
				}

			} else {
				// do not have a prevChild, then refer next directly
				// updated: the very first record on B+tree, and will not come to this case
				New->next = Cur->child[1];
				printf("this happens\n");
			}
		}

	}


	// children is full
	if ( Cur->key_num == MaxChildNumber ) {
		for ( i=0; i<Cur->key_num; i++ ) {
			printf(" %s,", Cur->key[i]);
		}
		printf(" Need Split\n");
		Split(Cur);
	}
}


/*
 *  Find a leaf node that key lays in it
 *	modify indicates whether key should affect the tree
 */
BPlusTreeNode* Find( char *key, int modify ) {
	BPlusTreeNode* Cur = Root;

	while (1) {
		if ( Cur->isLeaf == true ) {
			break;
		}

		if ( strcmp ( key, Cur->key[0] ) < 0 ) {
			if (modify == true)
				strcpy( Cur->key[0], key );
			Cur = Cur->child[0];
		} else {
			int i = Binary_Search(Cur, key);
			Cur = Cur->child[i];
		}
	}

	return Cur;
}



/** Print subtree whose root is Cur */
/*
void Print(BPlusTreeNode* Cur) {
	int i;
	for (i = 0; i < Cur->key_num; i++)
		printf("%d ", Cur->key[i]);
	printf("\n");
	if (!Cur->isLeaf) {
		for (i = 0; i < Cur->key_num; i++)
			Print(Cur->child[i]);
	}
}
*/

/** Interface: Insert (key, value) into B+tree */
int BPlusTree_Insert(char *key) {

	BPlusTreeNode* Leaf = Find( key, true );
	int i = Binary_Search(Leaf, key);
	if ( i == -1 ) {
		Insert( Leaf, key, NULL );
	}
	else if ( strcmp( Leaf->key[i], key ) == 0 ) {
		return false;
	}
	else {
		Insert( Leaf, key, NULL );
	}
	return true;
}

/** Interface: query all record whose key satisfy that key = query_key */
/*
void BPlusTree_Query_Key(unsigned int key) {
	BPlusTreeNode* Leaf = Find(key, false);
	QueryAnsNum = 0;
	int i;
	for (i = 0; i < Leaf->key_num; i++) {
		//printf("%d ", Leaf->key[i]);
		if (Leaf->key[i] == key) {
			QueryAnsNum++;
			if (QueryAnsNum < 20) printf("[no.%d, key = %u, value = %s]\n", QueryAnsNum, Leaf->key[i], (char*)Leaf->child[i]);
		}
	}
	printf("Total number of answers is: %d\n", QueryAnsNum);
}
*/

/** Interface: Find the position of given key */
// int BPlusTree_Find(int key) {
//
// 	BPlusTreeNode* Leaf = Find(key, false);
// 	int i = Binary_Search(Leaf, key);
//
// 	if ( strcmp(Leaf->key[i], key) != 0 )
// 		return -1; // don't have this key
//
// 	return Leaf->pos[i];
// }


/** Destroy subtree whose root is Cur, By recursion */
void Destroy(BPlusTreeNode* Cur) {
	if (Cur->isLeaf == true) {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			free(Cur->child[i]);
	} else {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			Destroy(Cur->child[i]);
	}
	free(Cur);
}


/** Interface: Called to destroy the B+tree */
void BPlusTree_Destroy() {
	if (Root == NULL) return;
	printf("Now destroying B+tree ..\n");
	Destroy(Root);
	Root = NULL;
	printf("Done.\n");
}


/** Interface: Initialize */
void BPlusTree_Init() {
	BPlusTree_Destroy();
	Root = New_BPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
	TotalNodes = 0;
}


/**
 * Interface: setting MaxChildNumber in your program
 * A suggest value is cube root of the no. of records
 */
void BPlusTree_SetMaxChildNumber(int number) {
	MaxChildNumber = number + 1;
}


/** Interface: print the tree (DEBUG use)*/
void BPlusTree_Print() {
	// struct BPlusTreeNode* Leaf = Find(1000000000, false);
	// int cnt = 0;
	// while (Leaf != NULL) {
	// 	int i;
	// 	for (i = Leaf->key_num - 1; i >= 0; i--) {
	// 		printf("%4d ", Leaf->key[i]);
	// 		if (++cnt % 20 == 0) printf("\n");
	// 	}
	// 	Leaf = Leaf->last;
	// }
	int i;
	printf("Block 1:");
	for ( i=0; i<Root->child[0]->key_num; i++ ) {
		printf(" %s,", Root->child[0]->key[i] );
	}
	printf("\n");
	printf("Block 2:");
	for ( i=0; i<Root->child[1]->key_num; i++ ) {
		printf(" %s,", Root->child[1]->key[i] );
	}
	printf("\n");
}


/** Interface: Total Nodes */
int BPlusTree_GetTotalNodes() {
	return TotalNodes;
}
