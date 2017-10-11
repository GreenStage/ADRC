#include <stdio.h>
#include <stdlib.h>


int main(){

    char command;
    char buffer[100];
    FILE * fp;


    printf("ADRC - First Mini Project\n");
    printf("\t\tMENU\n");
    printf("- Print table (p)\n");
    printf("- Lookup next-hop (n)\n");
    printf("- Insert new prefix (i)\n");
    printf("- Delete prefix (d)\n");
    
    sscanf(stdin, "%c", command);
    printf("oila");
    printf("you chose: %c\n", command);


    fp = fopen("example_tree.txt", "r");
    if (fp) {
        while (fscanf(fp, "%s", buffer)!=EOF)
            printf("%s",buffer);
        fclose(file);
    }else{
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}