#include "common.h"
#include "defs.h"
#include "tree.h"

int main(int argc, char * argv[]){
    int quit = 0;
    char command;
    char buffer[100];
    char line[100];
    char address[17];
    char extra[SMALL_STR_SIZE];
    char usage[][SMALL_STR_SIZE] = {
        "Usage: prefixer <file_name>",
        "The file must exist and contain a valid tree."
    };
    char help[][SMALL_STR_SIZE] = {
        "MENU",
        "\t- Print table (p)",
        "\t- Lookup next-hop (n)",
        "\t- Insert new prefix (i)",
        "\t- Delete prefix (d)",
        "\t- Binary tree to two bit (b)",
        "\t- Print table even (e)",
        "\t- Exit (q)"
    };

    Tree * prefixTree;
    /*Argument Checks*/
    if(argc < 2){
        foreach(usage,sizeof(usage[0]),arraylength(usage),(void*)(void*) printStr);
        exit(EXIT_MISSING_ARG);
    }

    prefixTree = tree_init(BINARY);
    PrefixTree(prefixTree,argv[1]);

    //TODO read tree

    printStr("ADRC - First Mini Project");
    foreach(help,sizeof(help[0]),arraylength(help),(void*)(void*) printStr);

    
    while(!quit){
        memset(extra,0,sizeof(char) * SMALL_STR_SIZE);

        fgets(line,100,stdin);
        sscanf(line, "%c %s", &command,extra);
        switch(command){
            case 'p':
                PrintTable(prefixTree);
                break;
            case 'n':

                memcpy(address,extra,sizeof(char) * 16);
                printf("Next Hop: %d\n",LookUp(prefixTree,extra));
                break;
            case 'i':
                break;
            case 'd':
                break;
            case 'b':
                prefixTree = BinaryToTwoBit(prefixTree);
                break;
            case 'e':
                break;
            case 'q':
                quit = 1;
                break;
            default:
                printStr("Invalid command.");
                foreach(help,sizeof(help[0]),arraylength(help),(void*)(void*) printStr);
                break;
        }
    }

    printf("You chose: %c\n",command);


    exit(EXIT_SUCCESS);
}