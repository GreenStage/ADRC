#include "common.h"
#include "defs.h"


int main(int argc, char * argv[]){
    int quit = 0;
    char command;
    char buffer[100];
    char line[100];
    FILE * fp;
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
        "\t- Exit (q)"
    };
    /*Argument Checks*/
    if(argc < 2){
        foreach(usage,SMALL_STR_SIZE* sizeof(char),sizeof(usage)/ (SMALL_STR_SIZE * sizeof(char)),(void*)(void*) printStr);
        exit(EXIT_MISSING_ARG);
    }
    //TODO read tree
    
    printStr("ADRC - First Mini Project");
    foreach(help,SMALL_STR_SIZE* sizeof(char),sizeof(help)/ (SMALL_STR_SIZE * sizeof(char)),(void*)(void*) printStr);

    while(!quit){
        fgets(line,100,stdin);
        sscanf(line, "%c", &command);
        switch(command){
            case 'p':
                PrintTable(...);
                break;
            case 'n':
                break;
            case 'i':
                break;
            case 'd':
                break;
            case 'q':
                quit = 1;
                break;
            default:
                printStr("Invalid command.");
                foreach(help,SMALL_STR_SIZE* sizeof(char),sizeof(help)/ (SMALL_STR_SIZE * sizeof(char)),(void*)(void*) printStr);
                break;
        }
    }

    printf("You chose: %c\n",command);


    exit(EXIT_SUCCESS);
}