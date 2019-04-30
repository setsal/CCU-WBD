#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>

struct Block {

};

struct Base {
    // int *ptr = malloc(sizeof(int));
};

unsigned char bytes[BASE_SIZE];

int main () {
    // int *blockptr = malloc(BLOCK_SIZE);
    // int *baseptr = malloc(BASE_SIZE);
    // int count = 1;

    // printf("%d\n", sizeof(char));
    // int n = 12345;
    unsigned char bytes[17];
    unsigned int n = 1;

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    printf("%x %x %x %x\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    // char* a = (char*)&n;
    // printf("%d")
    // free(ptr);
    return 0;
}

void IntTobinary ( long value ) {

}
