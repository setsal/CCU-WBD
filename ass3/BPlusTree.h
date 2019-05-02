#ifndef __BPlusTree_H__
#define __BPlusTree_H__

#define MAX_CHILD_NUMBER 3200

typedef struct BPlusTreeNode {
	int isRoot, isLeaf;
	int key_num;
	unsigned int key[MAX_CHILD_NUMBER];
	int pos[MAX_CHILD_NUMBER];
	void* child[MAX_CHILD_NUMBER];
	struct BPlusTreeNode* father;
	struct BPlusTreeNode* next;
	struct BPlusTreeNode* last;
} BPlusTreeNode;

extern void BPlusTree_SetMaxChildNumber(int);
extern void BPlusTree_Init();
extern void BPlusTree_Destroy();
extern int BPlusTree_Insert(unsigned int, int, void*);
extern int BPlusTree_GetTotalNodes();
extern void BPlusTree_Query_Key(unsigned int);
extern void BPlusTree_Print();

#endif
