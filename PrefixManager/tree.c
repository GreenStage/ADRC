#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "tree.h"

#define DEFAULT_ROOT_NODE -1

/*
 * Structure that represents a node on a prefix tree with 1 bit per node
 * */
typedef struct Tree_Node_ Tree_Node;
struct Tree_Node_
{
  /*Left is 0, Right is 1*/
  Tree_Node * left, * right; 
  int nextHop;
};

/*
 * Structure that represents a node on a prefix tree with 2 bits per node
 * */
typedef struct TwoBitTree_Node_ TwoBitTree_Node;
struct TwoBitTree_Node_
{
  TwoBitTree_Node * c[4];
  /*c1 is 00, c2 is 01, c3 is 10, c4 is 11 */
  int nextHop;
};

/*
 * Structure that stores que root of a tree and it's type.
 * */
struct Tree_{
  void * root;
  Tree_Type type;
};


typedef enum Tree_Parse_State_{
  EMPTY_LEFT,
  EMPTY_RIGHT,
} Tree_Parse_State;

/* Macro to print the information stored on each node on a two-bit tree.
 * Used for debugging
 */
#define PRINT_NODE_2BIT(N) \
do{ \
  printf("NODE %d - ",N->nextHop); \
  printf("Childs: 00 : %d, 01: %d , 10: %d, 11 : %d\n", \
   N->c[0] ? N->c[0]->nextHop : -2, \
   N->c[1] ? N->c[1]->nextHop : -2, \
   N->c[2] ? N->c[2]->nextHop : -2, \
   N->c[3] ? N->c[3]->nextHop : -2); \
  printf("--------------------------\n"); \
}while(0)

/*Macro to search through the tree*/
#define TREE_PARSE(A,P,B,O){ \
  int i; \
  Tree_Parse_State state; \
  Tree_Node * lastHop; \
  for(i = 0, lastHop = A; i < (int) strlen(B); i++){ \
      if(A->nextHop > 0) lastHop = A; \
      if(B[i] == '0'){ \
          P = A; \
          state = EMPTY_LEFT; \
          if(A->left == NULL){ \
              O; \
          } \
          else A = A->left; \
      } \
      else if(B[i] == '1'){ \
          P = A; \
          state = EMPTY_RIGHT; \
          if(A->right == NULL){ \
              O; \
          } \
          else A = A->right; \
      } \
      else break; \
  } \
}


/*function headers*/
Tree *  PrefixTree(char * file_name);
Tree_Node * createNewNode(Tree_Node * new_left,Tree_Node * new_right, int hopNumber);
void BinaryToTwoBit_sub(Tree_Node* ptr, TwoBitTree_Node  * new_ptr);
void PrintTableEven_sub(TwoBitTree_Node * t);
void PrintTable_sub(Tree_Node * t);
void BinaryTreeDestroy_sub(Tree_Node * t);
void TwoBitTreeDestroy_sub(TwoBitTree_Node * t);

/**************************************************************************** 
* NAME:        PrintTable_sub                                     
* DESCRIPTION: Recursive subroutine used to print each node
* ARGUMENTS:   node to print 
* RETURNS:     void
***************************************************************************/
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
    memcpy(auxStr,prefix, freePos * sizeof(char));

    printf("Hop: %d\t",t->nextHop);

    if(freePos == 0){
      PRINT_STR("OTHERS");
    }
    else PRINT_STR(auxStr);
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

/**************************************************************************** 
* NAME:        PrintTable                                     
* DESCRIPTION: Calls Printable_sub in order to print the table recursively. 
* ARGUMENTS:   binary prefix tree 
* RETURNS:     void
***************************************************************************/ 
void PrintTable(Tree * prefixTree){
  NULLPO_RETVE(prefixTree,"Error: tree is empty.");
  PrintTable_sub(prefixTree->root);
}

/**************************************************************************** 
* NAME:        PrintTableEven_sub                                    
* DESCRIPTION: recursively visits each node to print its information
* ARGUMENTS:   the node to print 
* RETURNS:     void
***************************************************************************/
void PrintTableEven_sub(TwoBitTree_Node * t){
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

    if(freePos == 0){
      PRINT_STR("OTHERS");
    } else PRINT_STR(auxStr);
  }

  if(t->c[0]){
    prefix[freePos++] = '0';
    prefix[freePos++] = '0';
    PrintTableEven_sub(t->c[0]);
  }
  freePos = pos;

  if(t->c[1]){
    prefix[freePos++] = '0';
    prefix[freePos++] = '1';
    PrintTableEven_sub(t->c[1]);
  }
  freePos = pos;

  if(t->c[2]){
    prefix[freePos++] = '1';
    prefix[freePos++] = '0';
    PrintTableEven_sub(t->c[2]);
  }
  freePos = pos;

  if(t->c[3]){
    prefix[freePos++] = '1';
    prefix[freePos++] = '1';
    PrintTableEven_sub(t->c[3]);
  }
  freePos = pos;

  return;
}

/**************************************************************************** 
* NAME:        PrintTableEven                                   
* DESCRIPTION: Calls PrintableEven_sub in order to print the table recursively. 
* ARGUMENTS:   args 
* RETURNS:     void
***************************************************************************/
void PrintTableEven( Tree * twoBitTree){
  NULLPO_RETVE(twoBitTree,"Error: Tree is empty!");
  ASSERT_RETVE(twoBitTree->type != TWOBIT,"Error: Tree is not Two-bit type.");

  PrintTableEven_sub(twoBitTree->root);
  return;
}

/**************************************************************************** 
* NAME:        tree_init                                     
* DESCRIPTION: creates a new tree
* ARGUMENTS:   type of the tree (binary or Two-bit) 
* RETURNS:     nelwy created tree
***************************************************************************/
Tree * tree_init(Tree_Type type){

  Tree * new;

  CREATE(new,1);
  NULLPO_RETRE(new,NULL,"Error allocating tree structure");

  new->root = NULL;
  new->type = type;
  return new;
}

/**************************************************************************** 
* NAME:        createNewNode                                     
* DESCRIPTION: creates a new node for a binary tree
* ARGUMENTS:   pointer to the left and right childs and the next hop for this node 
* RETURNS:     the newly created node
***************************************************************************/
Tree_Node * createNewNode(Tree_Node * new_left,Tree_Node * new_right, int nextHop){
  
  Tree_Node * newNode;
  
  CREATE(newNode,1);
  NULLPO_RETRE(newNode,NULL,"Error allocating memory for your new node.");


  newNode->left = new_left;
  newNode->right = new_right;
  newNode->nextHop = nextHop;
  
  return newNode;

}

/**************************************************************************** 
* NAME:        PrefixTree                                     
* DESCRIPTION: generates a prefix tree based on a table read from a text file
* ARGUMENTS:   name of the file with the prefix table 
* RETURNS:     a binary tree with prefixes and next-hops in each node
***************************************************************************/
Tree * PrefixTree(char * file_name){

  char line[100];
  Tree * prefixTree;
  FILE * fp;
  char prefix[25];
  int nextHop;

  prefixTree = tree_init(BINARY);
  NULLPO_RETR(prefixTree,NULL);
  NULLPO_RETR(file_name,NULL);

  /*check to see if there's already an existing tree*/
  if(prefixTree->root == NULL){
    prefixTree->root  = createNewNode(NULL, NULL, DEFAULT_ROOT_NODE);
  }

  /*open file and start reading to create tree*/
  fp = fopen(file_name, "r");
  NULLPO_RETRE(fp,NULL,"Error opening file.");
  
  while (fgets(line, sizeof(line), fp)) {
    #ifdef DEBUG
      printf("%s\n", line);
    #endif
    sscanf(line, "%s %d", prefix, &nextHop);

    if(strncmp(prefix,"D",1) == 0){
      Tree_Node * root = prefixTree->root;
      root->nextHop = nextHop;
    }
    else InsertPrefix(prefixTree, prefix, nextHop);

  }
  fclose(fp);
  return prefixTree;
}

/**************************************************************************** 
* NAME:        LookUp                                     
* DESCRIPTION: Given a prefix, finds it's the next-hop
* ARGUMENTS:   the prefix tree and and prefix we want to find 
* RETURNS:     the next hop for the given prefix
***************************************************************************/
int LookUp(Tree * prefixTree, char * address){
 
  Tree_Node * parent;
  Tree_Node * ptr = prefixTree->root;


  TREE_PARSE(ptr,parent,address,{
    return lastHop->nextHop;
  });

  return ptr->nextHop;
}

/**************************************************************************** 
* NAME:        InsertPrefix                                     
* DESCRIPTION: inserts a prefix and the associated next-hop in a prefix tree
* ARGUMENTS:   the prefix tree and the prefix and next-hop to insert   
* RETURNS:     the updated prefix tree
***************************************************************************/
Tree * InsertPrefix(Tree * prefixTree, char * prefix, int nextHop ){
  
  Tree_Node * newNode = NULL, *parent;
  Tree_Node * ptr = prefixTree->root;
 

  NULLPO_RETR(prefixTree,NULL);
  TREE_PARSE(ptr,parent,prefix,{ 
    
    switch(state){
      case EMPTY_RIGHT:
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        ptr->right = newNode;
        break;
      
      case EMPTY_LEFT:
        newNode = createNewNode(NULL, NULL, EMPTY_HOP);
        ptr->left = newNode;
        break;

    }
    ptr = newNode;
  });

  ptr->nextHop = nextHop;
  
  return prefixTree;

}

/**************************************************************************** 
* NAME:        DeletePrefix                                     
* DESCRIPTION: given a prefix, deletes it from the prefix tree
* ARGUMENTS:   the prefix tree and the prefix to be removed 
* RETURNS:     the updated prefix tree
***************************************************************************/
Tree * DeletePrefix(Tree * prefixTree, char * address){

  static Tree_Node * it = NULL;
  Tree_Node * parent;


  NULLPO_RETRE(prefixTree,NULL,"Error: Empty Tree");
  it = prefixTree->root;
  TREE_PARSE(it,parent,address,{});

  if(!it->left && !it->right){
   
    if(parent->left == it) parent->left = NULL;
    else parent->right = NULL;
    free(it);
  }

  else{

      it->nextHop = 0;
  }
  return prefixTree;
}  

/**************************************************************************** 
* NAME:        BinaryToTwoBit_sub                                     
* DESCRIPTION: recursive subroutine used to convert a binary tree into a Two-bit tree
* ARGUMENTS:   the node from the binary and the one from the two-bit tree 
* RETURNS:     void
***************************************************************************/
void BinaryToTwoBit_sub(Tree_Node* ptr, TwoBitTree_Node  * new_ptr){

  NULLPO_RETV(ptr);
  NULLPO_RETV(new_ptr);

  if(ptr->nextHop != 0) new_ptr->nextHop =ptr->nextHop;

  if(ptr->left){
    if(ptr->left->left){
      CREATE(new_ptr->c[0],1);
      new_ptr->c[0]->nextHop =ptr->left->nextHop;
      BinaryToTwoBit_sub(ptr->left->left,new_ptr->c[0]);
    }
    else if(ptr->left->nextHop != 0){
      CREATE(new_ptr->c[0],1);
      new_ptr->c[0]->nextHop = ptr->left->nextHop;
  #ifdef DEBUG      
        PRINT_NODE_2BIT(new_ptr->c[0]);
  #endif
    }

    if(ptr->left->right){
      CREATE(new_ptr->c[1],1);
      new_ptr->c[1]->nextHop = ptr->left->nextHop;
      BinaryToTwoBit_sub(ptr->left->right,new_ptr->c[1]);
    }
    else if(ptr->left->nextHop != 0){
      CREATE(new_ptr->c[1],1);
      new_ptr->c[1]->nextHop = ptr->left->nextHop;
  #ifdef DEBUG      
        PRINT_NODE_2BIT(new_ptr->c[1]);
  #endif      
    }
  }
  if(ptr->right){
    if(ptr->right->left){
      CREATE(new_ptr->c[2],1);
      new_ptr->c[2]->nextHop =   ptr->right->nextHop;
      BinaryToTwoBit_sub(ptr->right->left,new_ptr->c[2]);
    }
    else if(ptr->right->nextHop != 0){
      CREATE(new_ptr->c[2],1);
      new_ptr->c[2]->nextHop = ptr->right->nextHop;
  #ifdef DEBUG      
        PRINT_NODE_2BIT(new_ptr->c[2]);
  #endif            
    }

    if(ptr->right->right){
      CREATE(new_ptr->c[3],1);
      new_ptr->c[3]->nextHop = ptr->right->nextHop;
      BinaryToTwoBit_sub(ptr->right->right,new_ptr->c[3]);
    }   
    else if(ptr->right->nextHop != 0){
      CREATE(new_ptr->c[3],1);
      new_ptr->c[3]->nextHop = ptr->right->nextHop;
  #ifdef DEBUG      
        PRINT_NODE_2BIT(new_ptr->c[3]);
  #endif
    }
  }
  #ifdef DEBUG
        PRINT_NODE_2BIT(new_ptr);
  #endif
}

/**************************************************************************** 
* NAME:        BinaryToTwoBit                                     
* DESCRIPTION: calls a subroutine that will recursively convert a binary tree into a two-bit tree
* ARGUMENTS:   the binary tree to be converted 
* RETURNS:     the newly created two-bit tree
***************************************************************************/
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

/**************************************************************************** 
* NAME:        BinaryTreeDestroy_sub                                     
* DESCRIPTION: frees the allocated memory for the binary tree
* ARGUMENTS:   the node to be removed 
* RETURNS:     void
***************************************************************************/
void BinaryTreeDestroy_sub(Tree_Node * t){
  NULLPO_RETV(t);
  if(t->left){
    BinaryTreeDestroy_sub(t->left);
  }
  if(t->right){
    BinaryTreeDestroy_sub(t->right);
  }
  free(t);
}

/**************************************************************************** 
* NAME:        TwoBitTreeDestroy_sub                                     
* DESCRIPTION: frees the allocated memory for the Two-bit tree
* ARGUMENTS:   the node to be removed 
* RETURNS:     void
***************************************************************************/
void TwoBitTreeDestroy_sub(TwoBitTree_Node * t){
  NULLPO_RETV(t);
  FOREACH(ARRAY_LENGTH(t->c),{ TwoBitTreeDestroy_sub(t->c[iterator]);});
  free(t);
}

/**************************************************************************** 
* NAME:        Tree_Destroy                                     
* DESCRIPTION: calls recursive functions that will free the allocated memory
* ARGUMENTS:   the tree to be deleted 
* RETURNS:     void
***************************************************************************/
void Tree_Destroy(Tree * t){
  NULLPO_RETV(t);
  switch(t->type){

    case BINARY:
      BinaryTreeDestroy_sub(t->root);
      break;

    case TWOBIT:
      TwoBitTreeDestroy_sub(t->root);
      break;

    default:
      break;
  }
  free(t);
  return;
}