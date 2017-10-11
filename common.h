#ifndef COMMON_HEADER

#define COMMON_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printStr(char * str);
void foreach(void * structure,size_t hop_size,int num_it, void (*do_on_hop) (void * element));
#endif