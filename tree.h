#ifndef TREE_HEADER
#define TREE_HEADER

#include "defs.h"


typedef struct Tree_ Tree;

typedef enum Tree_Type_{
    BINARY,
    TWOBIT
} Tree_Type;

typedef enum Tree_Parse_State_{
    LEFT,
    RIGHT
} Tree_Parse_State;
void PrefixTree(Tree * prefrixTree, char * file_name);
Tree * InsertPrefix(Tree * root_node, char * prefix, int nextHop );

int LookUp(Tree * prefixTree,char * address);
void PrintTable(Tree * prefixTree);
Tree * tree_init(Tree_Type type);
Tree * BinaryToTwoBit(Tree * binaryTree);

#define TREE_PARSE(A,B,O){ \
    int i; \
    Tree_Parse_State state; \
    Tree_Node * aux, * lastHop; \
    for(i = 0, lastHop = A; i < (int) strlen(B); i++){ \
        aux = A; \
        if(A->nextHop > 0) lastHop = A; \
        if(B[i] == '0'){ \
            state = LEFT; \
            if(A->left == NULL){ \
                O; \
            } \
            else A = A->left; \
        } \
        else if(B[i] == '1'){ \
            state = RIGHT; \
            if(A->right == NULL){ \
                O; \
            } \
            else A = A->right; \
        } \
        else break; \
    } \
}

#endif