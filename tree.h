#ifndef TREE_HEADER
#define TREE_HEADER

#include "defs.h"

typedef struct Tree_ Tree;
void PrefixTree();
Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop );
Tree * createNewNode(Tree * new_left,Tree * new_right, int hopNumber);
int LookUp(char * address);
void PrintTable();
void tree_init();
#endif