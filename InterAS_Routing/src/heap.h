#ifndef HEAP_HEADER
#define HEAP_HEADER

#include "defs.h"

typedef unsigned item;


void heap_fix_up(item * array, unsigned int pos,bool (*comparator)(item,item));

void heap_fix_down(item * array, unsigned int pos, unsigned int nElements, bool (*comparator)(item,item));

#endif