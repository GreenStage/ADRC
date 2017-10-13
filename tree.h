#ifndef TREE_HEADER
#define TREE_HEADER

#include "defs.h"


typedef struct Tree_ Tree;

typedef enum Tree_Type_{
    BINARY,
    TWOBIT
} Tree_Type;

Tree * PrefixTree(char * file_name);
Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop );

int LookUp(Tree * prefixTree,char * address);
void PrintTable(Tree * prefixTree);

Tree * DeletePrefix(Tree * prefixTree, char * address);

Tree * BinaryToTwoBit(Tree * binaryTree);
void PrintTableEven( Tree * twoBitTree);

void Tree_Destroy(Tree * t);
#endif