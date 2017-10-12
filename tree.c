#include "common.h"

#include "tree.h"
/*
 * Structure that represents a node on a prefix tree with 1 bit per node
 * */
typedef struct Tree_Node_ Tree_Node;
struct Tree_Node_
{
  Tree_Node * left, * right; //left is 0, right is 1
  int nextHop;
};

/*
 * Structure that represents a node on a prefix tree with 2 bits per node
 * */
typedef struct TwoBitTree_Node_ TwoBitTree_Node;
struct TwoBitTree_Node_
{
  TwoBitTree_Node * c1, * c2, * c3, * c4;
  //c1 is 00, c2 is 01, c3 is 10, c4 is 11
  int nextHop;
};

/*
 * Structure that stores que root of a tree
 * */
struct Tree_{
  void * root;
  Tree_Type type;
};

//isto ainda é preciso ?
struct HorizontalBusElement{
  Tree_Node * ptr;
  void ( *instruction ) (void * arg);
  struct HorizontalBusElement * next;
};

//isto é só um header aqui perdido no meio ?
Tree_Node * createNewNode(Tree_Node * new_left,Tree_Node * new_right, int hopNumber);

struct HorizontalBusElement * mHorizontalBus;


/*
 * Recursive function used to visit each node and print the ones who have 
 * a next hop known to the router.
 * */
void PrintTable_sub(Tree_Node * t){
  static char prefix[17];
  static int freePos = 0;
  int pos;

  pos = freePos;
  
  NULLPO_RETV(t);
  ASSERT_RETV(freePos > 15);

  if(t->nextHop > 0){
    char auxStr[MAX_TREE_HEIGHT +1];

    memset(auxStr,'\0',MAX_TREE_HEIGHT);
    memcpy(auxStr,prefix, (freePos+1) * sizeof(char)) ;
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
/*
 * Function used to print the prefix table
 * */
void PrintTable(Tree * prefixTree){
  PrintTable_sub(prefixTree->root);
}

/*
 * Initialization of the tree
 * */
Tree * tree_init(Tree_Type type){

  Tree * new;

  CREATE(new,1);
  NULLPO_RETRE(new,NULL,"Error allocating tree structure");

  new->root = NULL;
  new->type = type;
  return new;
}

/*
 * Function that generates a prefix tree based on a table read from a 
 * text file.
 * */
void PrefixTree(Tree * prefrixTree, char * file_name){

  char line[100];
  FILE * fp;
  char prefix[25];
  int nextHop;

  NULLPO_RETV(prefrixTree);

  //check to see if there's already an existing tree
  if(prefrixTree->root == NULL){
    //create root node

    prefrixTree->root  = createNewNode(NULL, NULL, -1);

  }

  //open file and start reading to create tree
  fp = fopen("example_tree.txt", "r");

  if (fp) {
    while (fgets(line, sizeof(line), fp)) {
      #ifdef DEBUG
        printf("%s\n", line);
      #endif
      sscanf(line, "%s %d", prefix, &nextHop);

      InsertPrefix(prefrixTree->root , prefix, nextHop);

    }
      fclose(fp);
  }
  else{
      printf("Error opening file");
      exit(EXIT_FAILURE);
  }
  

}
/*
 * Function that returns the next-hop for a given address
 * */
int LookUp(Tree * prefixTree, char * address){
  static int i = 0;
  int retval;
  static Tree_Node * it = NULL;
  Tree_Node * ptr;

  ASSERT_RETR(i > (int) strlen(address) - 1, -1);

  if(it == NULL){
    it = prefixTree->root;
  } 

  ptr = it;
 
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
  i++;
  retval = LookUp(prefixTree,address++);
  i--;

  return retval == 0 ? ptr->nextHop : retval;

}

/*
 * Function used to insert a prefix and the associated next-hop
 * in a prefix tree
 * */
Tree * InsertPrefix(Tree * prefixTree, char * prefix, int nextHop ){
  int i;
  Tree_Node * auxNode;
  Tree_Node * nextNode;
  Tree_Node * newNode = NULL;

  NULLPO_RETR(prefixTree,NULL);
  auxNode = prefixTree->root;
  nextNode = auxNode;

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

  return prefixTree;

}

/*
 * Function that generates a new node for a given prefix tree
 * */
Tree_Node * createNewNode(Tree_Node * new_left,
                    Tree_Node * new_right, int nextHop){
  
  
  Tree_Node * newNode;
  
  CREATE(newNode,1);
  NULLPO_RETRE(newNode,NULL,"Error allocating memory for your new node.");


  newNode->left = new_left;
  newNode->right = new_right;
  newNode->nextHop = nextHop;
  
  return newNode;

}

//acho que nao precisa de ser recursiva
TwoBitTree_Node *  BinaryToTwoBit_sub(Tree_Node* ptr){
  static Tree_Node * parent = NULL;
  Tree_Node * mParent = parent;
  TwoBitTree_Node * new_ptr;
  NULLPO_RETR(ptr,NULL);

  parent = ptr;
  BinaryToTwoBit_sub(ptr->left);
  BinaryToTwoBit_sub(ptr->right);

  parent = mParent;

  CREATE(new_ptr,1);
  NULLPO_RETR(new_ptr,NULL);

  return new_ptr;
}

/*
 * Function used to convert a binary tree into a two bit prefix tree
 * */
Tree * BinaryToTwoBit(Tree * binaryTree){
  Tree_Node * ptr;
  TwoBitTree_Node * newRoot;
  Tree * retval;

  NULLPO_RETRE(binaryTree,NULL,"Error: Tree is null?");
  ASSERT_RETRE(binaryTree->type != BINARY,NULL,"Error: Tree is not binary");

  ptr = binaryTree->root;
  newRoot = BinaryToTwoBit_sub(ptr);
  
  CREATE(retval,1);
  NULLPO_RETR(retval,NULL);

  retval->root = newRoot;
  retval->type = TWOBIT;
  return retval;
}