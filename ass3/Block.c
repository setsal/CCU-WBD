#define BLOCK_SIZE 64000
#define BASE_SIZE 17

#include "Block.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void insert_into_init_block( Block *TargetBlock, unsigned char block[] );
void after_split_insert( Block *oldBlock, Block *newBlock, unsigned char base[] );
Block *split_block( Block *targetBlock, unsigned char base[] );
Block *start_new_block();



Block *start_new_block() {
	Block *new_block;
	new_block = malloc(sizeof(Block));
   	new_block->blockptr = malloc(BLOCK_SIZE);
    new_block->blockSize = 0;
    new_block->next = NULL;
    return new_block;
}

int find_insert_point( char *blockptr, int blockSize, unsigned char key[] ) {
    int position = 0;
    while ( strcmp( key, blockptr ) > 0 ) {
        position+=17;
        if ( position + 17 > blockSize ) {
            break;
        }
        blockptr+=17;
    }
    return position;
}

void insert_into_block( Block *TargetBlock, unsigned char base[] ) {

    char *TargetBlockPtr = TargetBlock->blockptr;
	char *basePtr = base;
    int insertPoint, i = 0;

	//Insert
	if ( TargetBlock->blockSize == 0 ) {

		// for first block
		while ( i < BASE_SIZE ) {
			*(TargetBlockPtr+i) = *(basePtr+i);
			i++;
		}
	}
	else {
		// find insert point to insert into block
		insertPoint = find_insert_point( TargetBlockPtr, TargetBlock->blockSize, base );
		if ( insertPoint == TargetBlock->blockSize ) {
			for ( i = 0; i < BASE_SIZE; i++ ) {
				*(TargetBlockPtr+insertPoint+i) = *(basePtr+i);
			}
		}
		else {
			// shift the value to new position
			for ( i = TargetBlock->blockSize-1; i>=insertPoint; i-- ) {
				*(TargetBlockPtr+i+17) = *(TargetBlockPtr+i);
			}

			//insert the new value
			for ( i = 0; i < BASE_SIZE; i++ ) {
				*(TargetBlockPtr+insertPoint+i) = *(basePtr+i);
			}
		}
	}
	// Update block size
	TargetBlock->blockSize = TargetBlock->blockSize + 17;
	// for ( i=0; i<TargetBlock->blockSize; i++ ) {
	//     if(i%17==0){
	//         printf("\n");
	//     }
	//     printf(" %x", *(TargetBlockPtr+i));
	// }
	// printf("\n---- \n");
}



void SplitBlock( Block *new, Block *old ) {

    int i;
    char *newptr = new->blockptr;
    char *oldptr = old->blockptr;

    oldptr = oldptr + 31994;

    for ( i=0; i<old->blockSize; i++ ) {
        *newptr++ = *oldptr++;
    }

    new->blockSize = old->blockSize - 31994;
    old->blockSize = 31994;
}


void after_split_insert( Block *oldBlock, Block *newBlock, unsigned char base[] ) {

	Block *targetBlock;
	char *basePtr = base;
	int i, insertPoint;

	if ( strcmp( newBlock->blockptr, base ) < 0 ) {
		targetBlock = newBlock;
	}
	else {
		targetBlock = oldBlock;
	}

	// find insert point to insert into block
	insertPoint = find_insert_point( targetBlock->blockptr, targetBlock->blockSize, base );
	if ( insertPoint == targetBlock->blockSize ) {
		for ( i = 0; i < BASE_SIZE; i++ ) {
			*(targetBlock->blockptr+insertPoint+i) = *(basePtr+i);
		}
	}
	else {
		// shift the value to new position
		for ( i = targetBlock->blockSize-1; i>=insertPoint; i-- ) {
			*(targetBlock->blockptr+i+17) = *(targetBlock->blockptr+i);
		}

		//insert the new value
		for ( i = 0; i < BASE_SIZE; i++ ) {
			*(targetBlock->blockptr+insertPoint+i) = *(basePtr+i);
		}
	}
	// Update block size
	targetBlock->blockSize = targetBlock->blockSize + 17;
}


Block *split_block( Block *targetBlock, unsigned char base[] ) {

	// Split the block
	Block *newBlock = malloc(sizeof(Block));
	newBlock->blockptr = malloc(BLOCK_SIZE);

	SplitBlock( newBlock, targetBlock );

	if ( targetBlock->next != NULL ) {
		newBlock->next = targetBlock->next;
	}
	else {
		newBlock->next = NULL;
	}
	after_split_insert( targetBlock, newBlock, base );
	
	// Link the old block to new
	targetBlock->next = newBlock;
	return newBlock;
}
