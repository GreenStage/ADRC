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

#define TREE_PARSE(A,B,O){ \
    int i; \
    for(i = 0; i < (int) strlen(B); i++){ \
        if(B[i] == '0'){ \
            if(A->left == NULL){ \
                O; \
            } \
            else A = A->left; \
        } \
        else if(B[i] == '1'){ \
            O; \
        } \
        else A = A->right; \
    } \
}

#endif