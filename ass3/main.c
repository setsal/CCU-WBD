#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>

struct Block {
    unsigned char *blockptr;
    int blockSize;
};


/*
    key  \0  id  \0
    11 + 1 + 4 + 1
*/
unsigned char base[BASE_SIZE];
unsigned char bytes[4];
long count = 1;


void *IntToByte( long value ) {

    bytes[0] = (int)((value >> 24) & 0xFF);
    bytes[1] = (int)((value >> 16) & 0xFF);
    bytes[2] = (int)((value >> 8) & 0xFF);
    bytes[3] = (int)(value & 0xFF);

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



int main () {

    unsigned char fakeinput[] = { "d4gKFHj4WdI" };


    IntToByte( count );
    InsertToBase( fakeinput );

    struct block tmp;
    tmp.blockptr = malloc(BLOCK_SIZE);



    FILE *pFile;
    pFile = fopen("../data/test.txt","wb");
    fwrite( &base, sizeof(char), 17,  pFile );
    fclose(pFile);


    return 0;
}
