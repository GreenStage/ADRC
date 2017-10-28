#include <string.h>
#include "defs.h"
#include "list.h"
#include "network.h"


enum link_type {
    PROVIDER,
    COSTUMER,
    PEER,
};
enum route_type{
    R_NONE = 0,
    R_PROVIDER,
    R_PEER,
    R_COSTUMER,
    R_SELF
};

typedef struct node_{
    list_node * links[3]; /*PROVIDER,COSTUMER,PEER*/
    int visitQueuePos; /* FOR FAST LOOKUP ON THE VISIT QUEUE ARRAY*/
} node;

typedef struct route_{
    enum route_type route_type;
    int nHops; 
}route;

struct graph_{
    /*Adjacency list instance, representing the graph */
    node * * nodes;

    /*Array of routes to a destination*/
    route * dest_route;
    int dest;
    
    bool * onStack;

    /*Array keeping track of unvisited nodes*/
    /*Visited nodes stack on the end of the array, separated from the 
     *unvisited ones by visitQueuePos*/
    int * visitQueue;

    /* "To visit" / "Visited" divider*/
    int visitQueuePos;

    /*Graph information*/
    bool isCommercial;
    int revisitAttempts;
    int costumerPaths, peerPaths, providerPaths, costumerCycles;
    int nodesCount, edgesCount;

};

graph * network_create_from_file(FILE * fp){
    graph * network;
    char line[100];
    int i,head, tail , type;

    int * appendNode;

    NULLPO_RETRE(fp,NULL,"File is null.");

    CREATE(network,1);
    NULLPO_RETRE(network,NULL,"Failed to create network.");


    CREATE(network->nodes,NETWORK_SIZE);
    NULLPO_RETRE(network->nodes,network,"Failed to create network.");

    CREATE(network->visitQueue,NETWORK_SIZE);
    NULLPO_RETRE(network->visitQueue,network,"Failed to create network.");

    i = 0;
    while( fgets(line,100,fp) ){
        i++;
        
        if( sscanf(line,"%d %d %d",&tail,&head,&type) < 3 ){
            printf("Error reading file line %d, skipping...",i);
            continue;
        }

        if(tail < 0  || type < 0 ||  type > 3 ||  head < 0|| tail >= NETWORK_SIZE || head >= NETWORK_SIZE){
            printf("Invalid link provided at line %d, skipping...",i);
            continue;
        }

        type--;

        CREATE(appendNode,1);
        if(appendNode == NULL){
            printf("Failed to create memory for tail node data %d at file line %d, skipping...",tail,i);
            continue;
        }

        *appendNode = tail;

        if(network->nodes[head] == NULL){
            CREATE(network->nodes[head],1);
            network->visitQueue[network->nodesCount] = head;
            network->nodes[head]->visitQueuePos = network->nodesCount;
            network->nodesCount++;
        }

        network->nodes[head]->links[type] = list_append(network->nodes[head]->links[type],(void *) appendNode);

        appendNode = NULL;

        CREATE(appendNode,1);
        if(appendNode == NULL){
            printf("Failed to create memory for head node data %d at file line %d, skipping...",head,i);
            continue;
        }

        *appendNode = head;

        type = (type == 2) ? 2 : 1 - type;

        if(network->nodes[tail] == NULL){
            CREATE(network->nodes[tail],1);
            network->visitQueue[network->nodesCount] = tail;
            network->nodes[tail]->visitQueuePos = network->nodesCount;
            network->nodesCount++;
        }

        network->nodes[tail]->links[type] = list_append(network->nodes[tail]->links[type],(void *) appendNode);

        network->edgesCount++;
    }

    network->visitQueuePos =  network->edgesCount -1;
    return network;
}

void visit(graph * network,int idx){
    int auxInt,auxPos;

    auxInt = network->visitQueue[network->visitQueuePos];
    auxPos = network->nodes[idx]->visitQueuePos;

    network->visitQueue[auxPos] = auxInt;
    network->visitQueue[network->visitQueuePos] = idx;

    network->nodes[idx]->visitQueuePos = network->visitQueuePos;
    network->nodes[auxInt]->visitQueuePos = auxPos;

    network->visitQueuePos--;

}

/*Returns number of direct and indirect costumers*/
void network_parse_costumers(graph * network, int index){
    list_node * aux;
    int peer;

    /*Ensure node exists*/
    NULLPO_RETV(network->nodes[index]);

    /*Ensure it was not visited*/
    ASSERT_RETV(network->nodes[index]->visitQueuePos > network->visitQueuePos);

    /*SWITCH*/
    visit(network,index);
    
    aux = network->nodes[index]->links[COSTUMER];
    LIST_PARSE(aux,{
        peer =  * ((int *) list_get_data(aux));
        if(network->onStack && network->onStack[peer]){
            network->revisitAttempts++;
#ifdef DEBUG
            printf("REVISIT ARRAY COLISION AT INDEX %d\n",index);
#endif
        }
        else if(network->nodes[peer]->visitQueuePos <= network->visitQueuePos){
            if(network->onStack) network->onStack[peer] = true;
            network_parse_costumers(network,peer);
            if(network->onStack) network->onStack[peer] = false;
        }

    });

    return;
}

void network_ensure_no_costumer_cycle(graph * network){
    int peer;

    NULLPO_RETVE(network,"ERROR: network_ensure_no_costumer cycle: network is null.");
    
    CREATE(network->onStack,NETWORK_SIZE);

    network->visitQueuePos = network->nodesCount -1;
    network->revisitAttempts = 0;

    for(peer = network->visitQueue[0]; network->visitQueuePos > -1 ; peer = network->visitQueue[0]){

        network->onStack[peer] = true;
        network_parse_costumers(network,peer);
        network->onStack[peer] = false;
        
        network->costumerCycles += network->revisitAttempts;
        network->revisitAttempts = 0;
    }
#ifdef DEBUG
    if( network->costumerCycles){
        printf("DEBUG - network_ensure_no_costumer_cycle: found %d costumer cycles.\n",network->costumerCycles);
    }
    else  printf("DEBUG - network_ensure_no_costumer_cycle: No costumer cycles found.\n");
  
#endif
    network->visitQueuePos = network->nodesCount -1;
    return;
}

void network_check_commercial(graph * network){
    int peer;
    list_node * ptr = NULL;

    NULLPO_RETVE(network,"Error - network_check_commercial: network is null.");
    ASSERT_RETV(network->costumerCycles > 0);

    network->visitQueuePos = network->nodesCount -1;
    network->revisitAttempts = 0;

     /*Fetch top provider, since it recursivily parses its costumers, decreases necessary iterations*/
    for(peer = network->visitQueue[0]; network->nodes[peer]->links[PROVIDER] != NULL;)   
        peer =  *( (int *) list_get_data(network->nodes[peer]->links[PROVIDER])) ;

    /*Fetch all direct and indirect costumers*/
    network_parse_costumers(network,peer);

    /*Fetch all direct and indirect costumers for each peer*/
    ptr = network->nodes[peer]->links[PEER];
    LIST_PARSE(ptr,{
        peer =  *( (int *) list_get_data(network->nodes[peer]->links[PEER])) ;
        network_parse_costumers(network,peer);
    });

#ifdef DEBUG
    printf("DEBUG - network_check_commercial: found %d disconnected nodes\n",network->visitQueuePos + 1);
#endif

    if(network->visitQueuePos == -1)
        network->isCommercial = true;
}

void network_update_dest_route(graph * network, int nodeIndex){
    static int nHops = 0;
    int id;
    list_node * ptr;
    
    network->dest_route[nodeIndex].nHops = nHops;
    nHops++;

    if(network->dest_route[nodeIndex].route_type >= R_COSTUMER){

        /*UPDATE PROVIDERS ROUTES*/
        ptr = network->nodes[nodeIndex]->links[PROVIDER];
        
        LIST_PARSE(ptr,{
            id = *((int *) list_get_data(ptr));

            if( (network->dest_route[id].route_type != R_SELF && network->dest_route[id].route_type != R_COSTUMER) 
             || (network->dest_route[id].route_type == R_COSTUMER && network->dest_route[id].nHops > nHops)){

                network->dest_route[id].route_type = R_COSTUMER;
#ifdef DEBUG
                printf("Node %d route updated to Costumer\n",id);
#endif
                network->dest_route[id].nHops = nHops;
                network_update_dest_route(network,id);
            }
        }); 

        /*UPDATE PEERS ROUTES*/
        ptr = network->nodes[nodeIndex]->links[PEER];
        LIST_PARSE(ptr,{
            id = *((int *) list_get_data(ptr));

            if(network->dest_route[id].route_type == R_NONE || network->dest_route[id].route_type == R_PROVIDER
            || (network->dest_route[id].route_type == R_PEER && network->dest_route[id].nHops > nHops)){

                network->dest_route[id].route_type = R_PEER;
#ifdef DEBUG
                printf("Node %d route updated to Peer\n",id);
#endif
                network->dest_route[id].nHops = nHops;
                network_update_dest_route(network,id);
            }
        });
    }

    if(!network->isCommercial){
        /*Only needed if the network is not commercially connected , see (1)*/
        ptr = network->nodes[nodeIndex]->links[COSTUMER];
    
        LIST_PARSE(ptr,{
            id = *((int *) list_get_data(ptr));

            if( network->dest_route[id].route_type == R_NONE 
             || (network->dest_route[id].route_type == R_PROVIDER && network->dest_route[id].nHops > nHops)){

                network->dest_route[id].route_type = R_PROVIDER;                
                network->dest_route[id].nHops = nHops;
                network_update_dest_route(network,id);
            }
        });
    }

    nHops--;
}


void network_find_paths_to(graph * network,int destination){

    NULLPO_RETVE(network,"Error - network_find_paths_to : network is null.");
    ASSERT_RETVE(destination >= NETWORK_SIZE,"Error - network_find_paths_to : destination not found in network.");
    NULLPO_RETVE(network->nodes[destination],"Error - network_find_paths_to : destination not found in network.");

    if(network->dest_route == NULL){
        CREATE(network->dest_route,NETWORK_SIZE);
    }
    else{
        memset(network->dest_route,0,NETWORK_SIZE * sizeof(route));
    }
    network->dest = destination;

    if(network->isCommercial){
        /*A network being commercially connected implies that each node, in the worst case scenario,
        * can connect to all the othes via a provider route, by initiliazing all routes type to "provider",
        * we can skip iterating over all costumers routes later on . (1) */
        FOREACH(NETWORK_SIZE,network->dest_route[iterator].route_type = R_PROVIDER );
    }

    network->dest_route[destination].route_type = R_SELF;
    network_update_dest_route(network,destination);
    printf("network_find_paths_to: Finishing calculating routes to %d.\n",destination);
}

void free_link(int * rt){
    free(rt);
}

void network_destroy(graph * network){
    NULLPO_RETV(network);

    if(network->nodes != NULL){
        FOREACH(NETWORK_SIZE,{
            if(network->nodes[iterator] == NULL) continue;

            list_free(network->nodes[iterator]->links[PROVIDER], (void *) free_link);
            list_free(network->nodes[iterator]->links[COSTUMER],(void *) free_link);
            list_free(network->nodes[iterator]->links[PEER],(void *) free_link);
            
            free(network->nodes[iterator]);
        });
    }

    if(network->dest_route){
        free(network->dest_route);
    }

    if(network->visitQueue){
        free(network->visitQueue);
    }

    free(network);
}

void routeType_toString(char * retval,enum route_type routeType){
    switch(routeType){

        case R_COSTUMER:
            sprintf(retval,"costumer route");
            break;
        case R_NONE:
            sprintf(retval,"no route");
            break;
        case R_PEER:
            sprintf(retval,"peer route");
            break;
        case R_PROVIDER:
            sprintf(retval,"provider route");
            break;
        default:
            sprintf(retval,"already at destination");
            break;         
    }
}
void network_create_log(graph * network, char * filename){
    FILE * fp;
    char routetype[][50] = {
        "has no route to destionation",
        "elects a provider route",
        "elects a peer route",
        "elects a costumer route",
        "is the destination itself"
    };

    fp = fopen(filename,"w");
    NULLPO_RETVE(fp,"Cannot open file for writing : %s", filename);

    fprintf(fp,"Network Info:\n");
    fprintf(fp,"\t%d Node, %d Edges\n",network->nodesCount,network->edgesCount); 

    if(network->costumerCycles){
        fprintf(fp,"\t%d costumer cycles found.\n",network->costumerCycles);      
    }  
    else fprintf(fp,"\tNo costumer cycles found.\n");
    
    if(network->isCommercial){
        fprintf(fp,"\tCommercially connected.\n");      
    }

    if(network->dest){
        fprintf(fp,"\n");
        fprintf(fp,"Route types elected to reach node %d:\n",network->dest);
        FOREACH(NETWORK_SIZE,{
            if(network->nodes[iterator] != NULL){
                fprintf(fp,"\t Node %d %s\n",iterator,routetype[network->dest_route[iterator].route_type]);
            }
        });
    }
    fclose(fp);
}