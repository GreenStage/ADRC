/*************Prefix Tree Manager *************/
/********** Instituto Superior Técnico ********
 * ADRC
 * Teacher: 
 *  João Sobrinho
 * Developed by :
 *  Eduardo Gomes and João Francisco Silva
 ******************09/13/2017******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "tree.h"

int main(int argc, char * argv[]){
    int quit = 0;
    char command;
    char line[MEDIUM_STR_SIZE];
    char address[MAX_TREE_HEIGHT + 1];
    char extra1[SMALL_STR_SIZE], extra2[SMALL_STR_SIZE];
    char usage[][SMALL_STR_SIZE] = {
        "Usage: prefixer <file_name>",
        "The file must exist and contain a valid tree."
    };
    char help[][SMALL_STR_SIZE] = {
        "MENU",
        "\t- Print table: p",
        "\t- Lookup next-hop: n <prefix>",
        "\t- Insert new prefix: n <prefix>",
        "\t- Delete prefix: d <prefix>",
        "\t- Binary tree to two bit: b",
        "\t- Print table even: e",
        "\t- Exit (q)"
    };

    Tree * prefixTree, *twoBitTree;

    /*Argument Checks*/
    if(argc < 2){
        FOREACH(ARRAY_LENGTH(usage), { PRINT_STR(usage[iterator]); });
        return EXIT_MISSING_ARG;
    }

    prefixTree = PrefixTree(argv[1]);

    if(prefixTree == NULL){ /*Problem reading from file, exit */
        FOREACH(ARRAY_LENGTH(usage), { PRINT_STR(usage[iterator]); });
        return EXIT_FAILURE;
    }

    /*Display menu*/
    PRINT_STR("ADRC - First Mini Project");
    FOREACH(ARRAY_LENGTH(help), { PRINT_STR(help[iterator]); });

    
    while(!quit){
        /*Clean buffers from previous iteration*/
        memset(extra1,0,sizeof(char) * SMALL_STR_SIZE);
        memset(extra2,0,sizeof(char) * SMALL_STR_SIZE);

        fgets(line,MEDIUM_STR_SIZE,stdin);
        sscanf(line, "%c %s %s", &command,extra1,extra2);

        switch(command){

            case 'p': /*Print prefix tree*/
                PrintTable(prefixTree);
                break;

            case 'n': /* Lookup a prefix*/
                if(EMPTY_STRING(extra1)){
                    printf("Invalid prefix.\n");
                }
                else{
                    memcpy(address,extra1,sizeof(char) * MAX_TREE_HEIGHT);
                    printf("Next Hop: %d\n",LookUp(prefixTree,extra1));
                }
                break;

            case 'i': /*Insert a prefix*/
                if(atoi(extra2) < 1){
                    printf("Invalid hop number\n");
                }
                else InsertPrefix(prefixTree,extra1,atoi(extra2));
                break;

            case 'd': /*Delete a prefix*/
                if(EMPTY_STRING(extra1)){
                    printf("Invalid prefix.\n");
                }
                else{
                    DeletePrefix(prefixTree,extra1);
                }
                break;

            case 'b': /*Convert binary tree to its Two bit counterpart*/
                twoBitTree = BinaryToTwoBit(prefixTree);
                if(twoBitTree) PRINT_STR("Conversion done.");
                break;

            case 'e': /*Prints an even prefix table , given the two bit table exists*/
                PrintTableEven(twoBitTree);
                break;

            case 'q': /*Exit*/
                quit = 1;
                break;

            default: /*Unknow command, print help strings*/
                PRINT_STR("Invalid command.");
                FOREACH(ARRAY_LENGTH(help),{ PRINT_STR(help[iterator]); });
                break;
        }
    }

    /*Free memory*/
    Tree_Destroy(prefixTree);
    Tree_Destroy(twoBitTree);

    exit(EXIT_SUCCESS);
}