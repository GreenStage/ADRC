#include <common.h>

typedef struct Tree 
{
  Tree * left, * right; //left is 0, right is 1
  int hopNumber;
  int bitNum;
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
  
  Tree * auxNode = root_node;
  Tree * nextNode = auxNode;
  Tree * newNode = NULL;

  for(int i = 0; /*prefix[i] != '\0'??*/ i < strlen(prefix); i++){

    auxNode = nextNode;

    if(strcmp(prefix[i], '0') == 0){
      //going left
      if(auxNode->left == NULL){
        newNode = createNewNode(newNode, NULL, NULL, EMPTY_HOP);
        auxNode->left = newNode;
        nextNode = newNode;
        
      }else{
        //there's already a node here!
        nextNode = auxNode->left;

      }

    }else{
      //going right
      if(auxNode->right == NULL){
        newNode = createNewNode(newNode, NULL, NULL, EMPTY_HOP);
        auxNode->right = newNode;
        nextNode = newNode;
        
      }else{
        //there's already a node here
        nextNode = auxNode->right;

      }

    }
    
  }
  /*When I exit this for , auxNode points to the last node inserted
    That means I can use it to store the hopNumber in the correct position
  */
  auxNode->hopNumber = nextHop;

  return root_node;

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