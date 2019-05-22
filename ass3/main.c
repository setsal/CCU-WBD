#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "BPlusTree.h"
#include "Block.h"


#define MIN_ORDER 3
#define MAX_ORDER 20
#define BUFFER_SIZE 256
#define bool char
#define false 0
#define true 1



/*
* First message to the user.
 */
void usage_1(void) {
    int order = DEFAULT_ORDER;
	printf("B+ Tree of Order %d.\n", order);
    printf("[Usage] bpt <order> <inputfile>\n");
}


/* Second message to the user.
 */
void usage_2(void) {
	printf("Enter any of the following commands after the prompt > :\n"
	"\ti <k>  -- Insert <k> (an integer) as both key and value).\n"
	"\ti <k> <v> -- Insert the value <v> (an integer) as the value of key <k> (an integer).\n"
	"\tf <k>  -- Find the value under key <k>.\n"
	"\tp <k> -- Print the path from the root to key k and its associated "
           "value.\n"
	"\td <k>  -- Delete key <k> and its associated value.\n"
	"\tx -- Destroy the whole tree.  Start again with an empty tree of the "
           "same order.\n"
	"\tt -- Print the B+ tree.\n"
	"\tl -- Print the keys of the leaves (bottom row of the tree).\n"
	"\tv -- Toggle output of pointer addresses (\"verbose\") in tree and "
           "leaves.\n"
	"\tq -- Quit. (Or use Ctl-D or Ctl-C.)\n"
	"\t? -- Print this help message.\n");
}


/* Brief usage note.
 */
void usage_3(void) {
	printf("Usage: ./bpt [<order>]\n");
	printf("\twhere %d <= order <= %d .\n", MIN_ORDER, MAX_ORDER);
}



int main (int argc, char ** argv) {

    	char * input_file;
    	FILE * fp, *pDataFile, *pDbFileMap, *pDbFile;
    	node *root;
        char str[1000];
        char input_key[20];
        unsigned int input_key_2;
    	char instruction;
        int i;
        unsigned int offset = 0;
        bool verbose_output = false;
        int order = DEFAULT_ORDER;

        //Block Use;
        unsigned int validRecords = 1;
        unsigned int pDbFileOffset, pDbFileMapOffset;
        char url[200], title[500], content[500], viewCount[10], res[10], duration[10];

    	root = NULL;


    	if (argc > 1) {
    		order = atoi(argv[1]);
    		if (order < MIN_ORDER || order > MAX_ORDER) {
    			fprintf(stderr, "Invalid order: %d .\n\n", order);
    			usage_3();
    			exit(EXIT_FAILURE);
    		}
    	}

        if (argc < 3) {
            usage_1();
            usage_2();
        }

    	if (argc > 2) {
            printf("Developing...\n");
            return EXIT_SUCCESS;
    	}

    	printf("[setsal DB]> ");
        char buffer[BUFFER_SIZE];
        int count = 0;
        bool line_consumed = false;
    	while (scanf("%c", &instruction) != EOF) {
            line_consumed = false;
    		switch (instruction) {
    		case 'd':
    			// scanf("%s", input_key);
    			// root = delete(root, input_key);
    			// print_tree(root);
    			break;
    		case 'i':
                fgets(buffer, BUFFER_SIZE, stdin);
                line_consumed = true;
                count = sscanf(buffer, "%s %u", input_key, &input_key_2);
    			root = insert(root, input_key, input_key_2);
    			print_tree(root);
    			break;
    		case 'f':
    		case 'p':
    			scanf("%s %u", input_key, &input_key_2 );
    			find_and_print(root, input_key, instruction == 'p');
    			break;
    		case 'r':
    			// scanf("%d %d", &input_key, &input_key_2);
    			// if (input_key > input_key_2) {
    			// 	int tmp = input_key_2;
    			// 	input_key_2 = input_key;
    			// 	input_key = tmp;
    			// }
    			// find_and_print_range(root, input_key, input_key_2, instruction == 'p');
    			// break;
    		case 'l':
    			print_leaves(root);
    			break;
    		case 'q':
    			while (getchar() != (int)'\n');
    			return EXIT_SUCCESS;
    			break;
            case 's':
                scanf("\n%s", input_key);
                find_and_print( root, input_key, 0 );
                break;
    		case 't':
    			print_tree(root);
    			break;
    		case 'v':
    			verbose_output = !verbose_output;
    			break;
    		case 'x':
    			if (root)
    				root = destroy_tree(root);
    			print_tree(root);
    			break;
            case 'j':
                printf("From File input b plus tree\n");
                pDataFile = fopen( "../data/youtube.rec.small","r");
                pDbFileMap = fopen("../data/mini/dbfilemap","w");
                pDbFile = fopen("../data/mini/dbfile","w");

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


                        //Insert into B Plus Tree
                        printf("[%d] ", validRecords);
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

                fclose(pDataFile);
                fclose(pDbFileMap);
                fclose(pDbFile);


                print_tree(root);
                break;
    		default:
    			usage_2();
    			break;
    		}
            if (!line_consumed)
               while (getchar() != (int)'\n');
    		printf("[setsal DB]> ");
    	}
    	printf("\n");

    	return EXIT_SUCCESS;

    return 0;
}
