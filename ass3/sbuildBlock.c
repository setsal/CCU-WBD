#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#define M 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


struct block {
    unsigned char *blockptr; //block data store
    struct block *next;
    int blockSize;
    unsigned int minValue;  //block min value
};
typedef struct block Block;
/*
    key  \0  id  \0
    11 + 1 + 4 + 1
*/
unsigned char base[BASE_SIZE];
unsigned char test2[BASE_SIZE];
unsigned char key[4];
long count = 1;
Block *head;

void IntToByte( unsigned int value );
void InsertToBase( unsigned char input[] );
unsigned int offset;
void InitBlock();
void InsertToBlock( unsigned char key[] );
void SplitBlock( Block *new, Block *old );
Block *FindBlock( unsigned char key[] );
int FindInsertPoint( char *blockptr, int blockSize, unsigned char key[] );



int main () {


    char str[1000];
    FILE *pDataFile;

    InitBlock();
    unsigned int validRecords = 1;


    pDataFile = fopen("../data/youtube.rec.mid","r");
    while(fgets(str, sizeof(str), pDataFile) != NULL) {
        if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
            str[48] = '\0';


            //create key value
            IntToByte(validRecords);

            //Create Small Base
            InsertToBase(str+37);

            //Insert to Block
            printf("Inserting %u record: %s", validRecords, str+37);
            printf(" %x %x %x %x, ", base[12], base[13], base[14], base[15]);
            InsertToBlock(str+37);
            printf("\n");
            validRecords++;
        }
    }
    fclose(pDataFile);

    //store in file
    FILE *fp;
    fp = fopen("../data/blockfile", "wb+");
    Block *curr = head;
    while( curr->next != NULL ) {
        fwrite(curr->blockptr, curr->blockSize, 1, fp);
        fwrite("\n", 1, 1, fp);
        curr = curr->next;
    }
    fclose(fp);

    return 0;
}


void IntToByte( unsigned int value ) {

    key[0] = (int)((value >> 24) & 0xFF);
    key[1] = (int)((value >> 16) & 0xFF);
    key[2] = (int)((value >> 8) & 0xFF);
    key[3] = (int)(value & 0xFF);
    //printf("%x %x %x %x\n", key[0], key[1], key[2], key[3]);

}

/*
    Base size : 17bytes
    Base format:  rid(11 bytes) \0 key(4 bytes) \0
*/
void InsertToBase( unsigned char rid[] ) {
    int i = 0;

    //record id
    for( i=0; i<11; i++ ) {
        base[i] = rid[i];
    }
    base[11] = '\0';

    //key value
    base[12] = key[0];
    base[13] = key[1];
    base[14] = key[2];
    base[15] = key[3];

    base[16] = '\0';
}


void InitBlock() {
    head = NULL;
    head = malloc(sizeof(Block));
    head->blockptr = malloc(BLOCK_SIZE);
    head->blockSize = 0;
    head->next = NULL;
}

Block *FindBlock( unsigned char key[] ) {
    Block *currentptr;
    Block *preptr;
    currentptr = head;

    if ( currentptr->next == NULL ) {  //First Block
        return head;
    }
    else {
        while ( strcmp( key, currentptr->blockptr ) > 0 ) {
            preptr = currentptr;
            currentptr = currentptr->next;
        }
        return preptr;
    }

}

void SplitBlock( Block *new, Block *old ) {
    int i;
    char *newptr = new->blockptr;
    char *oldptr = old->blockptr;

    oldptr = oldptr + 31994;
    for ( i=0; i<31994; i++ ) {
        *oldptr++ = *newptr++;
    }
    old->blockSize = old->blockSize - 31994;
    new->blockSize = 31994;
}

void InsertToBlock( unsigned char key[] ) {

    Block *TargetBlock = FindBlock(key);
    char *TargetBlockPtr = TargetBlock->blockptr;
    int i = 0;
    int insertPoint;

    if ( TargetBlock->blockSize + 17 > BLOCK_SIZE ) {

        printf(" need split\n");

        // Split the block
        Block *NewBlock = malloc(sizeof(Block));
        NewBlock->blockptr = malloc(BLOCK_SIZE);
        NewBlock->next = NULL;

        SplitBlock( NewBlock, TargetBlock );
        // Link the old block to new
        TargetBlock->next = NewBlock;

    }
    else {
        //Insert
        i = 0;
        if ( TargetBlock->blockSize == 0 ) {
            while ( i < BASE_SIZE ) {
                *(TargetBlockPtr+i) = base[i];
                i++;
            }
        }
        else {
            insertPoint = FindInsertPoint( TargetBlockPtr, TargetBlock->blockSize, key );
            if ( insertPoint == TargetBlock->blockSize ) {
                for ( i = 0; i < BASE_SIZE; i++ ) {
                    *(TargetBlockPtr+insertPoint+i) = base[i];
                }
            }
            else {

                // shift the value to new position
                for ( i = TargetBlock->blockSize-1; i>=insertPoint; i-- ) {
                    *(TargetBlockPtr+i+17) = *(TargetBlockPtr+i);
                }


                //insert the new value
                for ( i = 0; i < BASE_SIZE; i++ ) {
                    *(TargetBlockPtr+insertPoint+i) = base[i];
                }
            }
        }
        TargetBlock->blockSize = TargetBlock->blockSize + 17;
        // for ( i=0; i<TargetBlock->blockSize; i++ ) {
        //     if(i%17==0){
        //         printf("\n");
        //     }
        //     printf(" %x", *(TargetBlockPtr+i));
        // }
        // printf("\n---- \n");
    }
}

int FindInsertPoint( char *blockptr, int blockSize, unsigned char key[] ) {

    // printf("Current BlockSize: %d, (Default CompareValue: %s) v.s. (TargetValue: %s)\n", blockSize, blockptr, key);
    int position = 0;
    while ( strcmp( key, blockptr ) > 0 ) {

        position+=17;
        if ( position + 17 > blockSize ) {
            break;
        }
        blockptr+=17;
        // printf(" (New CompareValue: %s) v.s. (TargetValue: %s), position: %d\n", blockptr, key, position);

    }
    // printf("InsertPosition %d", position);
    return position;
}
