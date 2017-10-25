/************* Static Analysis of Inter-AS Routing *************/
/***************** Instituto Superior Técnico *******************
 * ADRC
 * Teacher: 
 *  João Sobrinho
 * Developed by :
 *  Eduardo Gomes and João Francisco Silva
 ************************10/20/2017*****************************/

#include <string.h>
#include "defs.h"
#include "network.h"

int main(int argc, char * argv[]){
    FILE * fp;
    graph * network;

    ASSERT_RETRE(argc < 2, EXIT_MISSING_ARG, "Missing input file name.");
    
    fp = fopen(argv[1],"r");
    NULLPO_RETRE(fp,EXIT_INVALID_FILE,"Cannot open file.");

    network = network_create_from_file(fp);
    fclose(fp);
    
    NULLPO_RETRE(network,EXIT_FAILURE,"Cannot create network from file.");
    
    network_ensure_no_costumer_cycle(network);

    network_check_commercial(network);
    
    network_destroy(network);

    return 0;

}