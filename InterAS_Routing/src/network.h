#ifndef NETWORK_HEADER
#define NETWORK_HEADER

#define NETWORK_SIZE 65651
#define MAX_DISTANCE 1000

enum calc_type{
    CALC_NONE = 0,
    CALC_TYPE = 0x1,
    CALC_HOPS = 0x2,
    CALC_ADVERTISER = 0x4,
    CALC_ALL = CALC_HOPS | CALC_ADVERTISER | CALC_TYPE
};

struct network_interface{
    bool (*create_from_file) (FILE * fp);
    bool (*ensure_no_costumer_cycle) (void);
    bool (*check_commercial) (void);
    void (*find_paths_to) (unsigned destination,enum calc_type flag);
    void (*print_log) (FILE * fp);
    void (*destroy) (void);
    void (*parse_all)(void);
};


struct network_interface * network_init();

#endif