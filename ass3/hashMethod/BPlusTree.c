#include "BPlusTree.h"
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

struct BPlusTreeNode* Root;

int MaxChildNumber = 50;
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
int Binary_Search(BPlusTreeNode* Cur, int key) {
	int l = 0, r = Cur->key_num;
	if (key < Cur->key[l]) return l;
	if (Cur->key[r - 1] <= key) return r - 1;
	while (l < r - 1) {
		int mid = (l + r) >> 1;
		if (Cur->key[mid] > key)
			r = mid;
		else
			l = mid;
	}
	return l;
}

/**
 * Cur(MaxChildNumber) split into two part:
 *	(1) Cur(0 .. Mid - 1) with original key
 *	(2) Temp(Mid .. MaxChildNumber) with key[Mid]
 * where Mid = MaxChildNumber / 2
 * Note that only when Split() is called, a new Node is created
 */
void Insert(BPlusTreeNode*, unsigned int, int, void*);
void Split(BPlusTreeNode* Cur) {
	// copy Cur(Mid .. MaxChildNumber) -> Temp(0 .. Temp->key_num)
	BPlusTreeNode* Temp = New_BPlusTreeNode();
	BPlusTreeNode* ch;
	int Mid = MaxChildNumber >> 1;
	Temp->isLeaf = Cur->isLeaf; // Temp's depth == Cur's depth
	Temp->key_num = MaxChildNumber - Mid;
	int i;
	for (i = Mid; i < MaxChildNumber; i++) {
		Temp->child[i - Mid] = Cur->child[i];
		Temp->key[i - Mid] = Cur->key[i];
		if (Temp->isLeaf) {
			Temp->pos[i - Mid] = Cur->pos[i];
		} else {
			ch = (BPlusTreeNode*)Temp->child[i - Mid];
			ch->father = Temp;
		}
	}
	// Change Cur
	Cur->key_num = Mid;
	// Insert Temp
	if (Cur->isRoot) {
		// Create a new Root, the depth of Tree is increased
		Root = New_BPlusTreeNode();
		Root->key_num = 2;
		Root->isRoot = true;
		Root->key[0] = Cur->key[0];
		Root->child[0] = Cur;
		Root->key[1] = Temp->key[0];
		Root->child[1] = Temp;
		Cur->father = Temp->father = Root;
		Cur->isRoot = false;
		if (Cur->isLeaf) {
			Cur->next = Temp;
			Temp->last = Cur;
		}
	} else {
		// Try to insert Temp to Cur->father
		Temp->father = Cur->father;
		Insert(Cur->father, Cur->key[Mid], -1, (void*)Temp);
	}
}

/** Insert (key, value) into Cur, if Cur is full, then split it to fit the definition of B+tree */
void Insert(BPlusTreeNode* Cur, unsigned int key, int pos, void* value) {
	int i, ins;
	if (key < Cur->key[0]) ins = 0; else ins = Binary_Search(Cur, key) + 1;
	for (i = Cur->key_num; i > ins; i--) {
		Cur->key[i] = Cur->key[i - 1];
		Cur->child[i] = Cur->child[i - 1];
		if (Cur->isLeaf) Cur->pos[i] = Cur->pos[i - 1];
	}
	Cur->key_num++;
	Cur->key[ins] = key;
	Cur->child[ins] = value;
	Cur->pos[ins] = pos;
	if (Cur->isLeaf == false) { // make links on leaves
		BPlusTreeNode* firstChild = (BPlusTreeNode*)(Cur->child[0]);
		if (firstChild->isLeaf == true) { // which means value is also a leaf as child[0]
			BPlusTreeNode* temp = (BPlusTreeNode*)(value);
			if (ins > 0) {
				BPlusTreeNode* prevChild;
				BPlusTreeNode* succChild;
				prevChild = (BPlusTreeNode*)Cur->child[ins - 1];
				succChild = prevChild->next;
				prevChild->next = temp;
				temp->next = succChild;
				temp->last = prevChild;
				if (succChild != NULL) succChild->last = temp;
			} else {
				// do not have a prevChild, then refer next directly
				// updated: the very first record on B+tree, and will not come to this case
				temp->next = Cur->child[1];
				printf("this happens\n");
			}
		}
	}
	if (Cur->key_num == MaxChildNumber) // children is full
		Split(Cur);
}


/** Find a leaf node that key lays in it
 *	modify indicates whether key should affect the tree
 */
BPlusTreeNode* Find(int key, int modify) {
	BPlusTreeNode* Cur = Root;
	while (1) {
		if (Cur->isLeaf == true)
			break;
		if (key < Cur->key[0]) {
			if (modify == true) Cur->key[0] = key;
			Cur = Cur->child[0];
		} else {
			int i = Binary_Search(Cur, key);
			Cur = Cur->child[i];
		}
	}
	return Cur;
}

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

/** Print subtree whose root is Cur */
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

/** Interface: Insert (key, value) into B+tree */
int BPlusTree_Insert(unsigned int key, int pos, void* value) {
	BPlusTreeNode* Leaf = Find(key, true);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] == key) return false;
	Insert(Leaf, key, pos, value);
	return true;
}

/** Interface: query all record whose key satisfy that key = query_key */
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

/** Interface: Find the position of given key */
int BPlusTree_Find(int key) {
	BPlusTreeNode* Leaf = Find(key, false);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return -1; // don't have this key
	return Leaf->pos[i];
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
	struct BPlusTreeNode* Leaf = Find(1000000000, false);
	int cnt = 0;
	while (Leaf != NULL) {
		int i;
		for (i = Leaf->key_num - 1; i >= 0; i--) {
			printf("%4d ", Leaf->key[i]);
			if (++cnt % 20 == 0) printf("\n");
		}
		Leaf = Leaf->last;
	}
}

/** Interface: Total Nodes */
int BPlusTree_GetTotalNodes() {
	return TotalNodes;
}
