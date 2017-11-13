#ifndef NETWORK_HEADER
#define NETWORK_HEADER

#define NETWORK_SIZE 65651

enum calc_type{
    CALC_NONE = 0,
    CALC_TYPE = 0x1,
    CALC_HOPS = 0x2,
    CALC_ADVERTISER = 0x4,
    CALC_ALL = CALC_HOPS | CALC_ADVERTISER | CALC_TYPE
};

typedef struct graph_ graph;

graph * network_create_from_file(FILE * fp);

void network_ensure_no_costumer_cycle(graph * network);
void network_check_commercial(graph * network);
void network_find_paths_to(graph * network, int destination,enum calc_type flag);
void network_print(graph * network,FILE * fp);
void network_destroy(graph * network);
void network_create_log(graph * network, char * filename);
#endif