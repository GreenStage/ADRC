#ifndef NETWORK_HEADER
#define NETWORK_HEADER

#define NETWORK_SIZE 65651

typedef struct graph_ graph;

graph * network_create_from_file(FILE * fp);

void network_ensure_no_costumer_cycle(graph * network);
void network_check_commercial(graph * network);
void network_find_paths_to(graph * network, int destination);
void network_print(graph * network,FILE * fp);
void network_destroy(graph * network);
#endif