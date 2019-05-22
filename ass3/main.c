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


/* Usage
 */
void usage(void) {
    printf("[INFO] B+ Tree of Order %d.\n", DEFAULT_ORDER);
	printf("[Usage] After start ./test, enter the commands after the prompt [setsal DB]>\n"
	"  i <record>  -- Insert the record value ( like @url:..@title... ).\n"
    "  s <key> -- Search & get the record value by record key id.\n"
	"  d <k>  -- Delete key <k> and its associated value.\n"
    "  j -- From file insert the record value and build the data structure.\n"
	"  x -- Destroy the whole tree.\n"
	"  t -- Print the B+ tree.\n"
	"  l -- Print the keys of the leaves (bottom row of the tree).\n"
	"  q -- Quit. (Or use Ctl-C.)\n"
	"  ? -- Print this help message.\n");
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
    int built = false;

    //Block Use;
    unsigned int validRecords = 1;
    unsigned int pDbFileOffset, pDbFileMapOffset;
    char url[200], title[500], content[500], viewCount[10], res[10], duration[10];

    root = NULL;


    if (argc > 1) {
        fprintf(stderr, "[Exception] Excute format Error. Program stop...\n\n");
        exit(EXIT_FAILURE);
    }
    if (argc < 3) {
        usage();
    }
    printf("\n[setsal DB]> ");
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
        case 'x':
            if (root)
                root = destroy_tree(root);
            printf("[INFO] Tree destory, try to print...\n");
            print_tree(root);
            built = false;
            break;
        case 'j':
            if (built == true) {
                printf("You have built the B+tree\n");
                break;
            }
            built = true;
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
            usage();
            break;
        }
        if (!line_consumed)
           while (getchar() != (int)'\n');
        printf("[setsal DB]> ");
    }
    printf("\n");

    return EXIT_SUCCESS;

}
