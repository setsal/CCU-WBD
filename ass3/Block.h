#ifndef __Block_H__
#define __Block_H__

typedef struct block {
    unsigned char *blockptr; //block data store
    struct block *next;
    int blockSize;
} Block;


extern Block *start_new_block();
extern void insert_into_block( Block *TargetBlock, unsigned char base[] );
extern void from_file_insert_tree( char filename[] );
extern Block *split_block( Block *targetBlock, unsigned char base[] );
#endif
