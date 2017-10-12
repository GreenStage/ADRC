#ifndef TREE_HEADER
#define TREE_HEADER

#include "defs.h"


typedef struct Tree_ Tree;

typedef enum Tree_Type_{
    BINARY,
    TWOBIT
} Tree_Type;

void PrefixTree(Tree * prefrixTree, char * file_name);
Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop );

int LookUp(Tree * prefixTree,char * address);
void PrintTable(Tree * prefixTree);
Tree * tree_init(Tree_Type type);
Tree * BinaryToTwoBit(Tree * binaryTree);

#endif