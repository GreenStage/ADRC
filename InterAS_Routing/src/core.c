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
    struct network_interface  * network;
    FILE * fp, *output;
    char line[100], command, extra[50], log_file_name[50];
    unsigned dest;
    bool quit = false;

    char help[][50]={
        "Usage:\n",
        "\tFind paths to a destination: n <destination>\n",
        "\tCalculate Statistics: s\n",
        "\tWrite log: l\n",
        "\tExit: q\n"
    };

    ASSERT_RETRE(argc < 2, EXIT_MISSING_ARG, "Missing input file name.");
    
    fp = fopen(argv[1],"r");
    NULLPO_RETRE(fp,EXIT_INVALID_FILE,"Cannot open file.");

    network = network_init();

    quit = !network->create_from_file(fp);
    fclose(fp);
    
    ASSERT_RETRE(quit == true,EXIT_FAILURE,"Cannot create network from file.");
    
    ASSERT_RETRE(!network->ensure_no_costumer_cycle(),EXIT_COSTUMER_CYCLE,"Network has costumer cycles.");
    
    if(network->check_commercial()){
        printf("Network is commercially connected.\n");
    }

    /*TODO PRINT READY*/
    while(!quit){
        FOREACH((int)sizeof(help)/sizeof(help[0]),printf("%s",help[iterator]));
        
        if(!fgets(line,MEDIUM_STR_SIZE,stdin)) break;
        sscanf(line, "%c %s", &command,extra);
        
        if(command == 'n'){
            if(sscanf(extra,"%u",&dest) > 0){
                enum calc_type type = CALC_NONE;
                printf("Find paths to node %d: , chose one or more options:\n", dest);
                printf("Multiple options are composed by the sums of their parts:\n");

                while(1){
                    printf("\tCalculate the types of each path to reach the destination (%d)\n",CALC_TYPE);
                    printf("\tCalculate number of hops in each path (%d)\n",CALC_HOPS);
                    printf("\tCalculate path advertisers (%d)\n",CALC_ADVERTISER);
                    printf("\tGo back (%d)\n",CALC_NONE);

                    if(!fgets(line,MEDIUM_STR_SIZE,stdin)){
                        quit = true;
                        break;
                    }
                    if(sscanf(line, "%u", &type)){
                        if(type >= CALC_NONE && type <= CALC_ALL) break;
                        printf("Invalid option %d \n", type);
                        printf("Usage:\n");
                    }
                }
                if(type == CALC_NONE) continue;
                
                network->find_paths_to(dest,type);
                    
                printf("network_find_paths_to: Finishing calculating routes to %d.\n",dest);

                while(1){
                    printf("Insert output log file name or \"stdout\" in order to print to the console. \n");
                    
                    if(!fgets(line,MEDIUM_STR_SIZE,stdin)){
                        quit = true;
                        break;
                    }
                    if(sscanf(line, "%s",extra)){
                        sprintf(log_file_name,"logs/%s",extra);
        
                        if(strcmp(extra,"stdout")){
                            output = fopen(log_file_name,"w");
                            if(output == NULL){
                                printf("Could not open %s for writing.\n",log_file_name);
                                continue;
                            }
                            network->print_log(output);
                            fclose(output);
                        }
                        else network->print_log(stdout);
                    }

                    break;
                }

            }
        }
        else if(command == 's'){
            network->parse_all();
            printf("Finished calculating statistics.\n");
        }
        else if(command == 'l'){
            printf("Insert output log file name or \"stdout\" in order to print to the console. \n");
            if(!fgets(line,MEDIUM_STR_SIZE,stdin)){
                quit = true;
                break;
            }

            if(sscanf(line, "%s",extra)){
                sprintf(log_file_name,"logs/%s",extra);

                if(strcmp(extra,"stdout")){
                    output = fopen(log_file_name,"w");
                    if(output == NULL){
                        printf("Could not open %s for writing.\n",log_file_name);
                        continue;
                    }
                    network->print_log(output);
                    fclose(output);
                }
                else network->print_log(stdout);
            }
        }
        else if(command == 'q'){
            quit = true;
        }
    }
    network->destroy();

    return 0;

}