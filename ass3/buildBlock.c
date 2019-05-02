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
unsigned char bytes[4];
long count = 1;
Block *head;

void IntToByte( unsigned int value );
void InsertToBase( unsigned char input[] );
unsigned int hashCode( char input[] );
unsigned int offset;
void InitBlock();
void InsertToBlock();
Block *FindBlock( unsigned int hashValue );



int main () {


    char str[1000];
    FILE *pDataFile;

    InitBlock();


    pDataFile = fopen("../data/youtube.rec.50","r");
    while(fgets(str, sizeof(str), pDataFile) != NULL) {
        if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
            str[48] = '\0';


            //create key value
            offset = ftell(pDataFile) - 49;
            printf("offset:%d\n", offset);
            IntToByte((unsigned int)offset);

            //Create Small Base
            InsertToBase(str+37);

            //Insert to Block
            printf("Inserting record %s\n",str+37);
            InsertToBlock();
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

    bytes[0] = (int)((value >> 24) & 0xFF);
    bytes[1] = (int)((value >> 16) & 0xFF);
    bytes[2] = (int)((value >> 8) & 0xFF);
    bytes[3] = (int)(value & 0xFF);
    //printf("%x %x %x %x\n", bytes[0], bytes[1], bytes[2], bytes[3]);

}


void InsertToBase( unsigned char input[] ) {
    int i = 0;

    // Key value
    for( i=0; i<11; i++ ) {
        base[i] = input[i];
    }

    base[11] = '\0';

    // ID
    base[12] = bytes[0];
    base[13] = bytes[1];
    base[14] = bytes[2];
    base[15] = bytes[3];

    base[16] = '\0';
}

unsigned int hashCode( char input[] ) {
    int i;
    unsigned int sum;
    for ( i=strlen(input); i>0; i-- ) {
        sum = sum + input[i]*((int)pow(31, i));
    }
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
        while ( hashValue > nextptr->minValue  ) {
            currentptr = currentptr->next;
            nextptr = currentptr->next;
        }
        return currentptr;
    }

}



void InsertToBlock() {

    unsigned int hashValue = hashCode(base);
    Block *ptr = FindBlock( hashValue );
    char *tmptr = ptr->blockptr;
    int i = 0;
    if ( ptr->blockSize + 17 > BLOCK_SIZE ) {
        printf("need split\n");
        //need split()
    }
    else {
        //Insert
        printf("%x %x %x %x\n", base[12], base[13], base[14], base[15]);
        i = 0;
        if ( ptr->blockSize == 0 ) {
            while ( i < BASE_SIZE ) {
                *(tmptr+i) = base[i];
                i++;
            }
        }
        else {
            tmptr = tmptr + ptr->blockSize;
            while ( i < BASE_SIZE ) {
                *(tmptr+i) = base[i];
                i++;
            }
        }
        ptr->blockSize = ptr->blockSize + 17;
    }
}
