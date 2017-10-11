#include "common.h"
#include "defs.h"


void printStr(char * str){
    printf("%s\n",str);
}
void foreach(void * structure,size_t hop_size,int num_it, void(*do_on_hop) (void * element) ) {
    int i;
    for(i = 0; i < num_it; i++){
        do_on_hop(structure + i * hop_size);
    }
    return;
}