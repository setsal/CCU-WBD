#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "BPlusTree.h"
#include "Block.h"
#include <WINSOCK2.H>

#define MIN_ORDER 3
#define MAX_ORDER 20
#define BUFFER_SIZE 256
#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#define bool char
#define false 0
#define true 1

// GLOBALS
unsigned char delete_array[100000][13];
unsigned char fullsearch_array[100000][13];
int fullsearch_array_counter;
char str[1000];
char ENV[6][50];
double start_time, end_time;
FILE *tmpfileptr;


//tries tree
char chr_legal[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJLMNOPQRSTUVWXYZ0123456789_-./";
int  chr_idx[256] = {0};
char idx_chr[256] = {0};

typedef struct trie_t *trie, trie_t;
struct trie_t {
	trie next[sizeof(chr_legal)]; /* next letter; slot 0 is for file name */
	int eow;
};


trie trie_new() { return calloc(sizeof(trie_t), 1); }
#define find_word(r, w) trie_trav(r, w, 1)

trie trie_trav(trie root, const char * str, int no_create)
{
	int c;
	while (root) {
		if ((c = str[0]) == '\0') {
			if (!root->eow && no_create) return 0;
			break;
		}
		if (! (c = chr_idx[c]) ) {
			str++;
			continue;
		}

		if (!root->next[c]) {
			if (no_create) return 0;
			root->next[c] = trie_new();
		}
		root = root->next[c];
		str++;
	}
	return root;
}

/**
 * Converts a string to lowercase.
 */
void strtolower(char *string) {
    int i;
    for (i = 0; i < strlen(string); i++) {
        string[i] = tolower(string[i]);
    }
}


void add_index(trie root, const char *word, const char *fname)
{
	trie x = trie_trav(root, word, 0);
	x->eow = 1;

	if (!x->next[0])
		x->next[0] = trie_new();
	x = trie_trav(x->next[0], fname, 0);
	x->eow = 1;
}

trie init_tables()
{
	int i, j;
	trie root = trie_new();
	for (i = 0; i < sizeof(chr_legal); i++) {
		chr_idx[(int)chr_legal[i]] = i + 1;
		idx_chr[i + 1] = chr_legal[i];
	}
    return root;
}

int print_path(char *path)
{

	fprintf(tmpfileptr, " %s", path);
	strcpy(fullsearch_array[fullsearch_array_counter], path);
	fullsearch_array_counter++;
	return 1;
}

int trie_all(trie root, char path[], int depth, int (*callback)(char *))
{
	int i;
	if (root->eow && !callback(path)) return 0;

	for (i = 1; i < sizeof(chr_legal); i++) {
		if (!root->next[i]) continue;

		path[depth] = idx_chr[i];
		path[depth + 1] = '\0';
		if (!trie_all(root->next[i], path, depth + 1, callback))
			return 0;
	}
	return 1;
}


void search_index(trie root, const char *word)
{
	char path[1024];
	fullsearch_array_counter = 0;
	tmpfileptr = fopen("info.txt", "w");
	fprintf(tmpfileptr, "Search for \"%s\": ", word);
	trie found = find_word(root, word);

	if (!found) printf("not found\n");
	else {
		trie_all(found->next[0], path, 0, print_path);
		printf("\n");
	}
	fclose(tmpfileptr);
}


void init_global() {
    printf("[INFO] Reading file location in .env file.\n");
    FILE *fp = fopen(".env", "r");
    char *loc;
    int i = 0;
    while( fscanf(fp, "%s\n", str) != EOF ) {
        loc = strchr(str, '=');
        loc++;
        strcpy(ENV[i], loc);
        i++;
        if ( i == 5 ) {
            break;
        }
    }
}



int check_not_in_delete( char *key, int counter ) {
    int i;
    for ( i=0; i<counter;i++) {
        if ( strcmp(key, delete_array[i]) == 0 ) {
            return false;
            printf("[INFO] Record not found.\n");
        }
    }
    return true;
}



int main (int argc, char ** argv) {

    char * input_file;
    FILE * fp, *pDataFile, *pDbFileMap, *pDbFile;
    node *root;

    unsigned int offset_array[100000];

    char input_key[20];
    unsigned int input_key_2;
    char instruction;
    int i;
    unsigned int offset = 0;
    bool verbose_output = false;
    int order = DEFAULT_ORDER;
    int built = false;
    int delete_counter = 0;

    //Block Use;
    unsigned int validRecords = 1;
    unsigned int pDbFileOffset, pDbFileMapOffset;
    char url[200], title[500], content[500], viewCount[10], res[10], duration[10];

    root = NULL;

    //tries Tree
    trie trieRoot = init_tables();
    char *splitPtr;


    if (argc > 1) {
        fprintf(stderr, "[Exception] Excute format Error. Program stop...\n\n");
        exit(EXIT_FAILURE);
    }
    if (argc < 3) {
        init_global();
    }
    printf("\n[setsal DB]> \n");

    // socket Used
    WSADATA wsaData;
    WORD versionRequired=MAKEWORD(1,1);
    WSAStartup(versionRequired,&wsaData);
    SOCKET sock_fd;
    SOCKADDR_IN  servaddr;
    SOCKADDR_IN  cliaddr;
    int len = sizeof(cliaddr);
    char buff[1024];
    time_t timep;
    sock_fd = socket(PF_INET,SOCK_DGRAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    memset(buff,0,sizeof(buff));
    bind(sock_fd, (SOCKADDR *)&servaddr, sizeof(servaddr));
    int receiveCounter = 0;

    char buffer[BUFFER_SIZE];
    int count = 0;



    while (1) {
        memset( buff, 0, sizeof(buff) );
        recvfrom( sock_fd, buff, sizeof(buff), 0, (struct sockaddr *)&cliaddr, &len);

		sscanf(buff, "%c %s", &instruction, input_key );
		printf("[INFO] Receive command %s\n", buff);

        switch (instruction) {
        case 'd':
            strcpy( delete_array[delete_counter], input_key );
            delete_counter++;
            printf("[Success] Delete record %s\n", input_key );
            print_tree(root);
			sprintf(buff, "%s",  "success");
            break;
        case 'i':
            fgets(buffer, BUFFER_SIZE, stdin);
            count = sscanf(buffer, "%s %u", input_key, &input_key_2);
            start_time = clock();
            root = insert(root, input_key, input_key_2);
            end_time = clock();
            printf("[INFO] Insert on a key, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
            print_tree(root);
			sprintf(buff, "%s",  "success");
            break;
        case 'l':
            print_leaves(root);
			sprintf(buff, "%s",  "success");
            break;
        case 'q':
            while (getchar() != (int)'\n');
			closesocket(sock_fd);
			WSACleanup();
            return EXIT_SUCCESS;
            break;
        case 's':
            if (built == false) {
                printf("You have not built the block b+tree\n");
                break;
            }
            start_time = clock();
            if ( check_not_in_delete(input_key, delete_counter) ) {
                find_and_print( root, input_key, 0, ENV[2], ENV[3] );
				sprintf(buff, "%s",  "success");
            }
			else {
				printf("[INFO] Not found\n");
				sprintf(buff, "%s",  "not found");
			}
            end_time = clock();
            printf("[INFO] Search on a key, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
            break;
        case 't':
            print_tree(root);
			sprintf(buff, "%s",  "success");
            break;
        case 'x':
            if (root)
                root = destroy_tree(root);
            printf("[INFO] Tree destory, try to print...\n");
            print_tree(root);
            built = false;
			sprintf(buff, "%s",  "success");
            break;
        case 'r':
            if (built == true) {
                printf("You have built the block b+tree\n");
                break;
            }
            fp = fopen( ENV[1], "r");
            unsigned int nblock = 0;
            start_time = clock();
            while( fscanf(fp, "%s %u %u\n", input_key, &input_key_2, &offset) != EOF ){
                printf("[INFO] Read block info: %s %u %u\n", input_key, input_key_2, offset );
                root = insert_from_blockfile( root, input_key, input_key_2 );
                offset_array[nblock] = offset;
                nblock++;
            }
            fclose(fp);
            printf("[INFO] Have %u blocks.\n", nblock);
            traversal_leaf_and_append_block(root, offset_array, ENV[0] );
            end_time = clock();
            printf("[INFO] Rebuild the block b plus tree, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
            built = true;
			sprintf(buff, "%s",  "success");
            break;
        case 'j':
            if (built == true) {
                printf("You have built the block b+tree\n");
                break;
            }
            built = true;
            printf("From File input b plus tree\n");
            pDataFile = fopen( ENV[4], "r");
            pDbFileMap = fopen( ENV[3], "w");
            pDbFile = fopen( ENV[2], "w");
            start_time = clock();
            while( fgets(str, sizeof(str), pDataFile) != NULL ) {

                if ( str[0] == '@' && str[1] == '\n' ) {

                    pDbFileOffset = ftell(pDbFile);

                    /* --- Create DB FILE --- */
                    fwrite(url, 1, strlen(url), pDbFile);

                    fputc('\t', pDbFile);

                    fwrite(title, 1, strlen(title), pDbFile);
                    splitPtr= strtok (title," ,.-");
                    while (splitPtr != NULL)
                    {
                      // printf ("variable - %s in key - %s\n", splitPtr, url);
                      add_index(trieRoot, splitPtr, url);
                      splitPtr = strtok (NULL, " ,.-");
                    }


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


                    //Insert into B Plus Tree
                    // printf("[%d] ", validRecords);
                    root = insert(root, url, pDbFileMapOffset);
                    // printf("%d %s\n", pDbFileMapOffset, url);
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
            end_time = clock();
            printf("[INFO] Insert from file to build the block b plus tree, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
            fclose(pDataFile);
            fclose(pDbFileMap);
            fclose(pDbFile);

            print_tree(root);
			sprintf(buff, "%s",  "success");
            break;
        case 'k':
            if (built == false) {
                printf("You have not built the block b+tree\n");
                break;
            }
            start_time = clock();
            search_index(trieRoot, input_key);
            end_time = clock();
			tmpfileptr = fopen("tmp.txt", "w");
			for ( i=0; i<fullsearch_array_counter; i++){
				printf("%s\n", fullsearch_array[i]);
				find_and_print2( root, fullsearch_array[i], 0, ENV[2], ENV[3], tmpfileptr );
			}
			fclose(tmpfileptr);
            printf("[INFO] Full text search on a word, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
			sprintf(buff, "%s",  "success");
            break;
        case 'w':
            traversal_leaf_and_write_blockfile(root, ENV[0], ENV[1]);
			sprintf(buff, "%s",  "success");
            break;
        default:
            break;
        };

		//socket return
		sendto( sock_fd, buff, strlen(buff), 0, (struct sockaddr *)&cliaddr, sizeof(servaddr) );
        printf("[INFO] Send %s - %d\n", buff, receiveCounter++);
    }
    printf("\n");

    return EXIT_SUCCESS;

}
