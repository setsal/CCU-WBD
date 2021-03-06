#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct block {
    unsigned char *blockptr; //block data store
    struct block *next;
    int blockSize;
};
typedef struct block Block;
/*
    key  \0  id  \0
    11 + 1 + 4 + 1
*/
unsigned char base[BASE_SIZE];
unsigned char key[4];
Block *head;

void IntToByte( unsigned int value );
void InsertToBase( unsigned char input[] );
void InitBlock();
void InsertToBlock( unsigned char key[] );
void SplitBlock( Block *new, Block *old );
Block *FindBlock( unsigned char key[] );
int FindInsertPoint( char *blockptr, int blockSize, unsigned char key[] );
void printBlock();


int main () {


    char str[1000];
    char url[200], title[500], content[500], viewCount[10], res[10], duration[10];
    char *ptr;
    FILE *pDataFile, *pDbFileMap, *pDbFile;

    InitBlock();
    unsigned int validRecords = 1;
    unsigned int pDbFileOffset, pDbFileMapOffset;


    pDataFile = fopen("../data/youtube.rec.mid","r");
    pDbFileMap = fopen("../data/mid/dbfilemap","w");
    pDbFile = fopen("../data/mid/dbfile","w");

    while( fgets(str, sizeof(str), pDataFile) != NULL ) {

        if ( str[0] == '@' && str[1] == '\n' ) {

            pDbFileOffset = ftell(pDbFile);

            /* --- Create DB FILE --- */
            fwrite(url, 1, strlen(url), pDbFile);
            fputc('\t', pDbFile);
            fwrite(title, 1, strlen(title), pDbFile);
            fputc('\t', pDbFile);
            fwrite(content, 1, strlen(content), pDbFile);
            fputc('\t', pDbFile);
            fwrite(viewCount, 1, strlen(viewCount), pDbFile);
            fputc('\t', pDbFile);
            fwrite(res, 1, strlen(res), pDbFile);
            fputc('\t', pDbFile);
            fwrite(duration, 1, strlen(duration), pDbFile);
            fputc('\n', pDbFile);


            /* --- Create DB FILE MAP --- */
            pDbFileMapOffset = ftell(pDbFileMap);
            fprintf( pDbFileMap, "%u\n", pDbFileOffset);


            /* --- Create Block FILE --- */
            // create key value
            IntToByte( pDbFileMapOffset );

            // Create Small Base
            InsertToBase(url);

            //Insert to Block
            printf("%d %s\n", pDbFileMapOffset, url);
            InsertToBlock(url);
            // printBlock();
            validRecords++;
        }
        else if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
            str[48] = '\0';
            strcpy( url, str+37 );

        }
        else if ( str[0] == '@' && str[1] == 't' && str[2] == 'i' && str[3] == 't' ) {
            strcpy( title, str+7 );
            title[strlen(title)-1] = '\0';
        }
        else if ( str[0] == '@' && str[1] == 'c' && str[2] == 'o' && str[3] == 'n' ) {
            strcpy( content, str+9 );
            content[strlen(content)-1] = '\0';
        }
        else if ( str[0] == '@' && str[1] == 'v' && str[2] == 'i' && str[3] == 'e' ) {
            strcpy( viewCount, str+11 );
            viewCount[strlen(viewCount)-1] = '\0';
        }
        else if ( str[0] == '@' && str[1] == 'r' && str[2] == 'e' && str[3] == 's' ) {
            strcpy( res, str+5 );
            res[strlen(res)-1] = '\0';
        }
        else if ( str[0] == '@' && str[1] == 'd' && str[2] == 'u' && str[3] == 'r' ) {
            strcpy( duration, str+10 );
            duration[strlen(duration)-1] = '\0';
        }
    }

    fclose(pDataFile);
    fclose(pDbFileMap);
    fclose(pDbFile);

    // write record in blockfile
    FILE *fp;
    fp = fopen("../data/mid/blockfile", "wb+");

    Block *curr = head;
    int i;
    while( curr != NULL ) {
        printf("key:%s, ", curr->blockptr);
        fwrite(curr->blockptr, curr->blockSize, 1, fp);
        for ( i=curr->blockSize; i<BLOCK_SIZE; i++) {
            fputc(0, fp);
        }
        printf("%d\n", ftell(fp));
        curr = curr->next;
    }
    fclose(fp);
    printf("Initial finish!\n");
    return 0;

}


void IntToByte( unsigned int value ) {

    key[0] = (int)((value >> 24) & 0xFF);
    key[1] = (int)((value >> 16) & 0xFF);
    key[2] = (int)((value >> 8) & 0xFF);
    key[3] = (int)(value & 0xFF);

    // printf("%u, %x %x %x %x\n", value, key[0], key[1], key[2], key[3]);

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
    preptr = currentptr;

    if ( currentptr->next == NULL ) {  //First Block
        return head;
    }
    else {
        while ( strcmp( key, currentptr->blockptr + currentptr->blockSize - BASE_SIZE ) > 0  ) {
            // preptr = currentptr;
            currentptr = currentptr->next;
            if ( currentptr->next == NULL ) {
                return currentptr;
            }
        }
        return currentptr;
    }

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

void InsertToBlock( unsigned char key[] ) {

    Block *TargetBlock = FindBlock(key);

    char *TargetBlockPtr = TargetBlock->blockptr;
    int i = 0;
    int insertPoint;

    if ( TargetBlock->blockSize + 17 > BLOCK_SIZE ) {

        // Split the block
        Block *NewBlock = malloc(sizeof(Block));
        NewBlock->blockptr = malloc(BLOCK_SIZE);

        SplitBlock( NewBlock, TargetBlock );

        if ( TargetBlock->next != NULL ) {
            NewBlock->next = TargetBlock->next;
        }
        else {
            NewBlock->next = NULL;
        }
        // Link the old block to new
        TargetBlock->next = NewBlock;
        InsertToBlock(key);

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

// debug use
void printBlock() {
    printf("\n -- [BLOCK STATUS] -- \n");
    int i;
    Block *TargetBlock = head;
    while ( TargetBlock != NULL ) {
        for ( i=0; i<TargetBlock->blockSize; i++ ) {
            if(i%17==0){
                printf("\n");
            }
            printf(" %c", *( TargetBlock->blockptr + i ));
        }
        printf("\n ---- \n");
        TargetBlock = TargetBlock->next;
    }
}
