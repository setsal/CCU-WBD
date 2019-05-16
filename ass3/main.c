#define BLOCK_SIZE 64000
#define BASE_SIZE 17
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "BPlusTree.h"


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
	"\tr <k1> <k2> -- Print the keys and values found in the range "
			"[<k1>, <k2>\n"
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
    	FILE * fp;
    	node *root;
        unsigned char str[100];
        char input_key[20];
        unsigned int input_key_2;
    	char instruction;
        int i;
        unsigned int offset = 0;
        bool verbose_output = false;
        int order = DEFAULT_ORDER;

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
            printf("From File input b plus tree\n");
    		input_file = argv[2];
    		fp = fopen(input_file, "r");
    		if (fp == NULL) {
    			perror("Failure to open input file.");
    			exit(EXIT_FAILURE);
    		}


            //get the file size & calculate the total block
            fseek(fp, 0, SEEK_END);
            int total_block = ftell(fp)/BLOCK_SIZE;
            rewind(fp);

            //insert the block min value
            for ( i=1; i<=total_block; i++ ) {
                fgets (str, 18, fp);
                printf("Trying to insert:%s %u\n", str, offset);
                root = insert(root, str, offset);
                fseek(fp, 64000*i, SEEK_SET);
                offset = ftell(fp);
            }

    		fclose(fp);
    		print_tree(root);
            // return EXIT_SUCCESS;
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
    		default:
    			usage_2();
    			break;
    		}
            if (!line_consumed)
               while (getchar() != (int)'\n');
    		printf("> ");
    	}
    	printf("\n");

    	return EXIT_SUCCESS;

    return 0;
}
