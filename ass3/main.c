#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#define M 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "BPlusTree.h"

#define true 1
#define false 0
#define MAX_KEY 1000000000

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

void *IntToByte( long value );
void InsertToBase( unsigned char input[] );

// file
char input_file[100];
char output_file[100];
char *buffer;
int fsize;
// record
int new_key, new_pos, new_len;
char new_st[12];
// data
const int TotalRecords = 10000000;
int validRecords;
// test
int keys[10000000], key_num;


/** Read and insert records into B+tree */
void Read_Data_And_Insert();

/** Modify (key, value) on data file */
int File_Modify(int pos, int key, char *value);

/** Delete (key, value) on data file */
void File_Delete(int pos);

/** Insert (key, value) on data file */
int File_Insert(int new_key, char* new_st);

/** Show Help */
void ShowHelp();
void MainLoop();


int main () {

    //
    // unsigned char key[12];
    //
    // // IntToByte( count );
    // // InsertToBase( fakeinput );
    // char str[1000];
    // FILE *pDataFile;
    //
    // int i = 0;
    //
    //
    //
    // pDataFile = fopen("../data/youtube.rec.50","r");
    // while(fgets(str, sizeof(str), pDataFile) != NULL) {
    //     if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
    //         str[48] = '\0';
    //         strcpy(key, str+37);
    //         count++;
    //     }
    // }
    // fclose(pDataFile);

    // set input_file, output_file
	strcpy(input_file, "data/small-data.txt");
	strcpy(output_file, "data/out.txt");

	// MainLoop (for presentation)
	MainLoop();

    return 0;
}


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




/** Read and insert records into B+tree */
void Read_Data_And_Insert() {
	int rid = 0;
	int cur = 0;
    char key[12];

    // IntToByte( count );
    // InsertToBase( fakeinput );
    char str[1000];
    FILE *pDataFile;

    int i = 0;

    pDataFile = fopen("../data/youtube.rec.50","r");
    while(fgets(str, sizeof(str), pDataFile) != NULL) {
        if( str[0] == '@' && str[1] == 'u' && str[2] == 'r' && str[3] == 'l' ){
            str[48] = '\0';
            rid++;
            char* value = (char*)malloc(sizeof(char) * 12);
            strcpy(value, str+37);
            printf("Inserting the %d th record, value:%s\n", rid, key);
            if (BPlusTree_Insert(rid, 30, value) == true) validRecords++;
        }
    }
}

/** Show Help */
void ShowHelp() {
	printf("\nType your operation:\n");
	printf("  0) Test Initialize\n");
	printf("  1) Set Depth\n");
	printf("  2) Set MaxChildNumber\n");
	printf("  3) Build Tree\n");
	printf("  4) Query on a key\n");
	printf("  9) Quit\n");
}

void MainLoop() {
	double start_time, end_time;
	int built = false;

	// B+tree initialize
	BPlusTree_Init();
	while (1) {
		ShowHelp();
		int request;
		scanf("%d", &request);
		switch (request) {
			case 0: {
				break;
			}
			case 1: {
				// Set Depth
				printf("input depth: ");
				int depth;
				scanf("%d", &depth);
				int maxCh = 2;
				while (1) {
					int leaves = 1, i;
					for (i = 0; i < depth; i++) {
						leaves *= maxCh;
						if (leaves > TotalRecords) break;
					}
					if (leaves > TotalRecords) break;
					maxCh++;
				}
				printf("Desired depth = %d, calculated maxChildNumber = %d\n", depth, maxCh);
				BPlusTree_SetMaxChildNumber(maxCh);
				break;
			}
			case 2: {
				// Set MaxChildNumber
				printf("input MaxChildNumber: ");
				int maxCh;
				scanf("%d", &maxCh);
				BPlusTree_SetMaxChildNumber(maxCh);
				break;
			}
			case 3: {
				// Build B+tree
				if (built == true) {
					printf("You have built the B+tree\n");
					break;
				}
				built = true;
				Read_Data_And_Insert();
				printf("Valid Records inserted on B+tree = %d\n", validRecords);
				printf("Total number of B+tree nodes = %d\n", BPlusTree_GetTotalNodes());
				break;
			}
			case 4: {
				// Query on a key
				printf("input the key: ");
				int key;
				scanf("%d", &key);
				start_time = clock();
				BPlusTree_Query_Key(key);
				end_time = clock();
				printf("Query on a key, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
				break;
			}
			case 9: return;
			default: break;
		}
	}
	BPlusTree_Destroy();
}
