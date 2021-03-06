#include <string.h>
#include "defs.h"
#include "heap.h"


void heap_fix_up(item * array, unsigned pos,bool (*comparator)(item,item)){
    item aux;
    while(pos > 0 && comparator(array[pos],array[ (pos-1) >> 1 ])){
        memcpy(&aux,&array[pos],sizeof(item));
        memcpy(&array[pos],&array[ (pos-1) >> 1 ],sizeof(item));
        memcpy(&array[ (pos-1) >> 1],&aux,sizeof(item));
        pos = (pos-1) >> 1;
    }
}

void heap_fix_down(item * array, unsigned pos, unsigned nElements, bool (*comparator)(item,item)){
    item aux;
    unsigned i,j;
    
    if(nElements == 0) return;

    for(i = pos; (i << 1) + 1 < nElements - 1; i = j){
        j = (i << 1) + 1;

        if( j + 1 < nElements - 1 && comparator(array[j + 1],array[j]) ){
            j++;
        }

        if(!comparator(array[j],array[i])){
            break;
        }

        memcpy(&aux,&array[i],sizeof(item));
        memcpy(&array[i],&array[j],sizeof(item));
        memcpy(&array[j],&aux,sizeof(item));
    }
}
