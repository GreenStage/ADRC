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
  TwoBitTree_Node * c[4];
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


typedef enum Tree_Parse_State_{
  LEFT,
  RIGHT,
} Tree_Parse_State;

#define TREE_PARSE(A,P,B,O){ \
  int i; \
  Tree_Parse_State state; \
  Tree_Node * lastHop; \
  for(i = 0, lastHop = A; i < (int) strlen(B); i++){ \
      if(A->nextHop > 0) lastHop = A; \
      if(B[i] == '0'){ \
          P = A; \
          state = LEFT; \
          if(A->left == NULL){ \
              O; \
          } \
          else A = A->left; \
      } \
      else if(B[i] == '1'){ \
          P = A; \
          state = RIGHT; \
          if(A->right == NULL){ \
              O; \
          } \
          else A = A->right; \
      } \
      else break; \
  } \
}


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
    memcpy(auxStr,prefix, freePos * sizeof(char)) ;
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
void PrefixTree(Tree * prefixTree, char * file_name){

  char line[100];
  FILE * fp;
  char prefix[25];
  int nextHop;

  NULLPO_RETV(prefixTree);

  //check to see if there's already an existing tree
  if(prefixTree->root == NULL){
    //create root node

    prefixTree->root  = createNewNode(NULL, NULL, -1);

  }

  //open file and start reading to create tree
  fp = fopen("example_tree.txt", "r");

  if (fp) {
    while (fgets(line, sizeof(line), fp)) {
      #ifdef DEBUG
        printf("%s\n", line);
      #endif
      sscanf(line, "%s %d", prefix, &nextHop);

      InsertPrefix(prefixTree, prefix, nextHop);

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
 
  Tree_Node * parent;
  Tree_Node * ptr = prefixTree->root;


  TREE_PARSE(ptr,parent,address,{
    return lastHop->nextHop;
  });

  return ptr->nextHop;
}

/*
 * Function used to insert a prefix and the associated next-hop
 * in a prefix tree
 * */
Tree * InsertPrefix(Tree * prefixTree, char * prefix, int nextHop ){
  
  Tree_Node * newNode = NULL, *parent;
  Tree_Node * ptr = prefixTree->root;
 

  NULLPO_RETR(prefixTree,NULL);
  TREE_PARSE(ptr,parent,prefix,{ 
    
    switch(state){
      case RIGHT:
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        ptr->right = newNode;
        break;
      
      case LEFT:
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        ptr->left = newNode;
        break;

    }
    ptr = newNode;
  });

  ptr->nextHop = nextHop;
  
  return prefixTree;

}

/*
 * Function that generates a new node for a given prefix tree
 * */
Tree_Node * createNewNode(Tree_Node * new_left,Tree_Node * new_right, int nextHop){
  
  Tree_Node * newNode;
  
  CREATE(newNode,1);
  NULLPO_RETRE(newNode,NULL,"Error allocating memory for your new node.");


  newNode->left = new_left;
  newNode->right = new_right;
  newNode->nextHop = nextHop;
  
  return newNode;

}


Tree * DeletePrefix(Tree * prefixTree, char * address){

  //int retval;
  static Tree_Node * it = NULL;
  Tree_Node * parent;
  //Tree_Node * ptr;

  NULLPO_RETRE(prefixTree,NULL,"Error: Empty Tree");
  it = prefixTree->root;
  TREE_PARSE(it,parent,address,{});

  if(!it->left && !it->right){
    //this node has no children, it can be FREE
    if(parent->left == it) parent->left = NULL;
    else parent->right = NULL;
    free(it);
  }
  else{
      //this node has some children, it needs to be here for them
      it->nextHop = 0;
  }
  return prefixTree;
}  


void BinaryToTwoBit_sub(Tree_Node* ptr, TwoBitTree_Node  * new_ptr){
  static Tree_Node * parent = NULL;
  Tree_Node * mParent = parent;


  NULLPO_RETV(ptr);
  NULLPO_RETV(new_ptr);

  if(ptr->nextHop > 0) new_ptr->nextHop =ptr->nextHop;

  if(ptr->left){
    CREATE(new_ptr->c[0],1);
    CREATE(new_ptr->c[1],1);
    new_ptr->c[0]->nextHop = new_ptr->c[1]->nextHop = ptr->left->nextHop;

    if(ptr->left->left){
      BinaryToTwoBit_sub(ptr->left->left,new_ptr->c[0]);
    }
    if(ptr->left->right){
      BinaryToTwoBit_sub(ptr->left->right,new_ptr->c[1]);
    }   
  }
  if(ptr->right){
    CREATE(new_ptr->c[2],1);
    CREATE(new_ptr->c[3],1);
    new_ptr->c[2]->nextHop = new_ptr->c[3]->nextHop = ptr->right->nextHop;

    if(ptr->right->left){
      BinaryToTwoBit_sub(ptr->right->left,new_ptr->c[2]);
    }
    if(ptr->right->right){
      BinaryToTwoBit_sub(ptr->right->right,new_ptr->c[3]);
    }   
  }
#ifdef DEBUG
  printf("NODE %d - ",new_ptr->nextHop);
  printf("Childs: 00 : %d, 01: %d , 10: %d, 11 : %d\n",
   new_ptr->c[0] ? new_ptr->c[0]->nextHop : -2,
   new_ptr->c[1] ? new_ptr->c[1]->nextHop : -2,
   new_ptr->c[2] ? new_ptr->c[2]->nextHop : -2,
   new_ptr->c[3] ? new_ptr->c[3]->nextHop : -2);
  printf("--------------------------\n");
  return;
#endif
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

  CREATE(newRoot,1);
  BinaryToTwoBit_sub(ptr,newRoot);
  
  CREATE(retval,1);
  NULLPO_RETR(retval,NULL);

  retval->root = newRoot;
  retval->type = TWOBIT;
  return retval;
}