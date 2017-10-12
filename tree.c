#include "common.h"

#include "tree.h"

struct Tree_
{
  Tree * left, * right; //left is 0, right is 1
  int nextHop;
};

struct HorizontalBusElement{
  Tree * ptr;
  void ( *instruction ) (void * arg);
  struct HorizontalBusElement * next;
};

Tree * root_node;
struct HorizontalBusElement * mHorizontalBus;

void PrintTable_sub(Tree * t){
  static char prefix[17];
  static int freePos = 0;
  int pos;

  pos = freePos;
  
  if(t == NULL || freePos > 15){
    return;
  }

  if(t->nextHop > 0){
    char auxStr[MAX_TREE_HEIGHT +1];

    memset(auxStr,'\0',MAX_TREE_HEIGHT);
    memcpy(auxStr,prefix, (freePos+1) *sizeof(char)) ;
    printf("Hop: %d\t",t->nextHop);
    printStr(auxStr);
  }

  if(t->left){
    prefix[freePos++] = '0';
    PrintTable_sub(t->left);
  }
  freePos = pos;

  if(t->right){
    prefix[freePos++] = '1';
    PrintTable_sub(t->right);  
  }
  freePos = pos;

  return;
}
void PrintTable(){
  PrintTable_sub(root_node);
}
/*
void printBusRow(HorizontalBusElement * ptr){
  Tree * treePtr;
  while(ptr != NULL){
    treePtr = (Tree *) ptr;
    printf("%d\n",nextHop);

  }
}
*/
void tree_init(){
  root_node = NULL;
}

void PrefixTree(){

  char line[100];
  FILE * fp;
  char prefix[25];
  int nextHop;

  //check to see if there's already an existing tree
  if(root_node == NULL){
    //create root node

    root_node = createNewNode(NULL, NULL, -1);

  }

  //open file and start reading to create tree
  fp = fopen("example_tree.txt", "r");

  if (fp) {
    while (fgets(line, sizeof(line), fp)) {
      #ifdef DEBUG
        printf("%s\n", line);
      #endif
      sscanf(line, "%s %d", prefix, &nextHop);

      InsertPrefix(root_node, prefix, nextHop);

    }
      fclose(fp);
  }
  else{
      printf("Error opening file");
      exit(EXIT_FAILURE);
  }
  

}

int LookUp(char * address){
  int i;
  int retval;
  static Tree * it = NULL;
  Tree * ptr;

  if(it == NULL) it = root_node;
  ptr = it;
  for(i = 0; i < (int) strlen(address) ; i++){
    switch(address[i]){
      case '0':
        if(!it->left){
          return it->nextHop;
        }
        it= it->left;
        break;
      case '1':
        if(!it->right){
          return it->nextHop;
        }
        it= it->right;
        break;
      default:
        return -1;

    }
    retval = LookUp(address++);
    return retval == 0 ? ptr->nextHop : it->nextHop;
  }
}

Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop ){
  int i;
  Tree * auxNode = root_node;
  Tree * nextNode = auxNode;
  Tree * newNode = NULL;

  for(i = 0; /*prefix[i] != '\0'??*/ i < (int) strlen(prefix); i++){

    if(prefix[i] == '0'){
      //going left
      if(auxNode->left == NULL){
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        auxNode->left = newNode;
        nextNode = newNode;
        
      }else{
        //there's already a node here!
        nextNode = auxNode->left;

      }

    } else{
      //going right
      if(auxNode->right == NULL){
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        auxNode->right = newNode;
        nextNode = newNode;
        
      }else{
        //there's already a node here
        nextNode = auxNode->right;

      }

    }
    
    auxNode = nextNode;
  }
  /*When I exit this for , auxNode points to the last node inserted
    That means I can use it to store the nextHop in the correct position
  */
  auxNode->nextHop = nextHop;

  return root_node;

}

Tree * createNewNode(Tree * new_left,
                    Tree * new_right, int nextHop){
  
  
  Tree * newNode = (Tree*) malloc(sizeof(Tree));

  if(newNode == NULL){
    printf("Error allocating memory for your new node. Exitting.\n");
    exit(EXIT_FAILURE);
  }

  newNode->left = new_left;
  newNode->right = new_right;
  newNode->nextHop = nextHop;
/*
  if(newNode->nextHop){
    struct HorizontalBusElement * mElement, *aux;
    mElement = (struct HorizontalBusElement*) malloc(sizeof(struct HorizontalBusElement));
    memset(mElement,0,sizeof(struct HorizontalBusElement));
    for( aux = mHorizontalBus[floorNumber]; aux->next != NULL; aux = aux->next);
    aux->next = mElement;
    mElement->ptr = newNode;
  }
*/

  
  return newNode;

}