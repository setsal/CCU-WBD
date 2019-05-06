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
unsigned int hashCode( char input[] );
unsigned int offset;
void InitBlock();
void InsertToBlock( unsigned int hashValue );
void SplitBlock( Block *new, Block *old );
Block *FindBlock( unsigned int hashValue );
int FindInsertPoint( char *blockptr, int blockSize, unsigned int hashValue );



int main () {


    char str[1000];
    FILE *pDataFile;

    InitBlock();
    unsigned int validRecords = 1;


    pDataFile = fopen("../data/youtube.rec.50","r");
    while(fgets(str, sizeof(str), pDataFile) != NULL) {
        if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
            str[48] = '\0';


            //create key value
            IntToByte(validRecords);

            //Create Small Base
            InsertToBase(str+37);

            //Insert to Block
            printf("Inserting %u record: %s, hashValue: %u\n", validRecords, str+37, hashCode(str+37));
            InsertToBlock( hashCode(str+37) );

            validRecords++;
            if(validRecords == 3) {
                break;
            }
        }
    }
    fclose(pDataFile);

    //store in file
    FILE *fp;
    fp = fopen("local", "wb+");
    fwrite(head->blockptr, head->blockSize, 1, fp);
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

unsigned int hashCode( char input[] ) {
    int i;
    unsigned int sum;

    for ( i=strlen(input)-1; i>=0; i-- ) {
        printf("%x ", input[i]);
        sum = sum + input[i]*((int)pow(31, i));
    }

    printf("\n%s, length %d, sum %u\n", input, strlen(input), sum);
    return sum;
}

void InitBlock() {
    head = NULL;
    head = malloc(sizeof(Block));
    head->blockptr = malloc(BLOCK_SIZE);
    head->blockSize = 0;
    head->next = NULL;
}

Block *FindBlock( unsigned int hashValue ) {
    Block *currentptr;
    Block *nextptr;
    currentptr = head;
    nextptr = currentptr->next;

    if ( nextptr == NULL ) {  //First Block
        return head;
    }
    else {
        while ( hashValue > hashCode(nextptr->blockptr)  ) {
            currentptr = currentptr->next;
            nextptr = currentptr->next;
        }
        return currentptr;
    }

}

void SplitBlock( Block *new, Block *old ) {
    int i;
    char *newptr = new->blockptr;
    char *oldptr = old->blockptr;

    oldptr = oldptr + 31994;
    printf("\n");
    for ( i=0; i<31994; i++ ) {
        if( i<50 ) {
            if ( i%17 == 0 ) {
                printf("\n");
            }
            printf(" old char %x:", *oldptr );
        }
        *oldptr++ = *newptr++;
    }
    printf("\n\n");
    old->blockSize = old->blockSize - 31994;
    new->blockSize = 31994;
    // exit(-1);
}

void InsertToBlock( unsigned int hashValue ) {

    Block *TargetBlock = FindBlock( hashValue );
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
        printf(" %x %x %x %x\n", base[12], base[13], base[14], base[15]);
        i = 0;
        if ( TargetBlock->blockSize == 0 ) {
            while ( i < BASE_SIZE ) {
                *(TargetBlockPtr+i) = base[i];
                i++;
            }
        }
        else {
            insertPoint = FindInsertPoint( TargetBlockPtr, TargetBlock->blockSize, hashValue );

            for ( i = TargetBlock->blockSize-1; i>=insertPoint; i-- ) {
                // printf(" %x", *(TargetBlockPtr+i));
                *(TargetBlockPtr+i+17) = *(TargetBlockPtr+i);
            }
            printf("\n");
            for ( i = insertPoint; i < BASE_SIZE; i++ ) {
                // printf(" %x", *(TargetBlockPtr+i));
                *(TargetBlockPtr+i) = base[i];
            }

        }
        TargetBlock->blockSize = TargetBlock->blockSize + 17;
        for ( i=0; i<TargetBlock->blockSize; i++ ) {
            if(i%17==0){
                printf("\n");
            }
            printf(" %x", *(TargetBlockPtr+i));
        }
        printf("\n---- \n");
    }
}

int FindInsertPoint( char *blockptr, int blockSize, unsigned int hashValue ) {

    char tmpRid[11];
    strcpy( tmpRid, blockptr );
    unsigned int tmpRidHashValue = hashCode(blockptr);
    // printf("Current BlockSize: %d, (Default CompareValue: %s - %u) v.s. (TargetValue: %u)\n", blockSize, blockptr, tmpRidHashValue, hashValue);

    int position = 0;
    while ( hashValue > tmpRidHashValue ) {

        position+=17;
        if ( position + 17 > blockSize ) {
            break;
        }

        strcpy( tmpRid, blockptr+position );
        tmpRidHashValue = hashCode(tmpRid);
        printf(" (New CompareValue: %s - %u) v.s. (TargetValue: %u), position: %d\n", tmpRid, tmpRidHashValue, hashValue, position);

    }
    printf(" InsertPosition %d", position);
    return position;
}
