#ifndef NETWORK_HEADER
#define NETWORK_HEADER

#define NETWORK_SIZE 65651; 

typedef struct graph_ graph;

graph * network_create_from_file(FILE * fp);

void network_parse_customer_cycle(FILE * outputFp, graph * network);
void network_parse_commercially(FILE * outputFp, graph * network);
void network_parse_statistics(FILE * outputFp, graph * network);

void network_destroy(graph * network);
#endif