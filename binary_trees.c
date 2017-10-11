#include <common.h>

typedef struct Tree 
{
  Tree * left, * right; //left is 0, right is 1
  int hopNumber;
} Tree;

void PrintTable(Tree * t){

}

Tree * PrefixTree(){

  char line[100];
  FILE * fp;
  char prefix[25];
  int nextHop;
  Tree * root_node;

  //check to see if there's already an existing tree
  if(root_node == NULL){
    //create root node

    root_node = createNewNode(root_node, NULL, NULL, -1);
    //-1 means this is the root node
  }

  //open file and start reading to create tree
  fp = fopen("example_tree.txt", "r");
  if (fp) {
    while (fgets(line, sizeof(line), file)) {
      #ifdef DEBUG
        printf("%s", line);
      #endif
      sscanf(line, "%s %d", prefix, nextHop);

      root_node = InsertPrefix(root_node, prefix, nextHop);



    }
      fclose(file);
  }else{
      printf("Error opening file");
      exit(EXIT_FAILURE);
  }
  

}



Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop ){

  for(int i = 0; /*prefix[i] != '\0'??*/ i < strlen(prefix); i++){

    Tree * auxNode = root_node;

    if(strcmp(prefix[i], '0') == 0){
      //going left
      if(auxNode->left == NULL){
        newNode = createNewNode(newNode, NULL, NULL, EMPTY_HOP);
        auxNode->left = newNode;



        tenho de fazer isto com um aux node
      }

    }




  }

}

Tree * createNewNode(Tree * newNode,Tree * new_left,
                    Tree * new_right, int hopNumber){
  
  newNode = malloc(sizeof(Tree));
  if(newNode == NULL){
    printf("Error allocating memory for your new node. Exitting.\n");
    exit(EXIT_FAILURE);
  }
  auxNode->left = new_left;
  auxNode->right = new_right;
  auxNode->hopNumber = hopNumber;

  return newNode;

}