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
#include <time.h>

int main(int argc, char * argv[]){
    FILE * fp;
    char line[100], command, extra[50], log_file_name[50];
    int dest;
    graph * network;
    bool quit = false;

    ASSERT_RETRE(argc < 2, EXIT_MISSING_ARG, "Missing input file name.");
    
    fp = fopen(argv[1],"r");
    NULLPO_RETRE(fp,EXIT_INVALID_FILE,"Cannot open file.");

    network = network_create_from_file(fp);
    fclose(fp);
    
    NULLPO_RETRE(network,EXIT_FAILURE,"Cannot create network from file.");
    
    network_ensure_no_costumer_cycle(network);

    network_check_commercial(network);
    
    while(!quit){

        fgets(line,MEDIUM_STR_SIZE,stdin);
        sscanf(line, "%c %s", &command,extra);

        if(command == 'n'){
            if(sscanf(extra,"%d",&dest) > 0){
                network_find_paths_to(network,dest);
                while(1){
                    printf("Would you like to create a log? (y / n) \n");
                    fgets(line,MEDIUM_STR_SIZE,stdin);
                    sscanf(line, "%c", &command); 
                    if(command == 'y'){
                        struct tm *time_s;
                        char * p = log_file_name;
                        time_t rawtime = time(NULL);
                        time_s = gmtime(&rawtime);
                        strftime(log_file_name, sizeof(log_file_name), "log_%Y-%m-%d_%H:%M:%S.txt", time_s);
                        for (; *p; ++p)
                        {
                            if (*p == ' ')
                                  *p = '_';
                        }
                        network_create_log(network,log_file_name);
                        break;
                    }
                    else if(command == 'n') break;
                }

            }
        }
        else if(command == 'l'){
            if(sscanf(extra,"%s",log_file_name) > 0){
                network_create_log(network,log_file_name);
            }
        }
        else if(command == 'q'){
            quit = true;
        }
    }
    network_destroy(network);

    return 0;

}