
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "tree.h"

int main(int argc, char * argv[]){
    int quit = 0;
    char command;
    char line[100];
    char address[17];
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
        foreach(arraylength(usage), { PRINT_STR(usage[iterator]); });
        return EXIT_MISSING_ARG;
    }

    prefixTree = PrefixTree(argv[1]);
    if(prefixTree == NULL){
        foreach(arraylength(usage), { PRINT_STR(usage[iterator]); });
        return EXIT_FAILURE;
    }

    PRINT_STR("ADRC - First Mini Project");
    foreach(arraylength(help), { PRINT_STR(help[iterator]); });

    
    while(!quit){
        memset(extra1,0,sizeof(char) * SMALL_STR_SIZE);
        memset(extra2,0,sizeof(char) * SMALL_STR_SIZE);
        fgets(line,100,stdin);
        sscanf(line, "%c %s %s", &command,extra1,extra2);
        switch(command){
            case 'p':
                PrintTable(prefixTree);
                break;
            case 'n':
                if(EMPTY_STRING(extra1)){
                    printf("Invalid prefix.\n");
                }
                else{
                    memcpy(address,extra1,sizeof(char) * 16);
                    printf("Next Hop: %d\n",LookUp(prefixTree,extra1));
                }
                break;
            case 'i':
                if(atoi(extra2) < 1){
                    printf("Invalid hop number\n");
                }
                else InsertPrefix(prefixTree,extra1,atoi(extra2));
                break;
            case 'd':
                if(EMPTY_STRING(extra1)){
                    printf("Invalid prefix.\n");
                }
                else{
                    DeletePrefix(prefixTree,extra1);
                }
                break;
            case 'b':
                twoBitTree = BinaryToTwoBit(prefixTree);
                break;
            case 'e':
                PrintTableEven(twoBitTree);
                break;
            case 'q':
                quit = 1;
                break;
            default:
                PRINT_STR("Invalid command.");
                foreach(arraylength(help),{ PRINT_STR(help[iterator]); });
                break;
        }
    }

    Tree_Destroy(prefixTree);
    Tree_Destroy(twoBitTree);

    exit(EXIT_SUCCESS);
}