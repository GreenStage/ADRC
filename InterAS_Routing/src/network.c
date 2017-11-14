#include <string.h>
#include "defs.h"
#include "list.h"
#include "network.h"
#include "heap.h"

enum link_type {
    COSTUMER,
    PEER,
    PROVIDER
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
    int advertiser;
}route;

typedef struct graph_{
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
    enum calc_type flag;

} graph;

graph * network_data = NULL;

bool routecmp(route * r1,route * r2){

    NULLPO_RETR(r1,false);
    NULLPO_RETR(r2,true);

    if( ( r1->route_type == r2->route_type && r1->nHops < r2->nHops ) 
        || r1->route_type > r2->route_type) {
            
        return true;
    }
    else return false;
} 

bool routecmpById(int idx1,int idx2){
    return routecmp(&network_data->dest_route[idx1],&network_data->dest_route[idx2]);
} 

bool network_create_from_file(FILE * fp){
    char line[100];
    int i,head, tail , type;

    int * appendNode;

    NULLPO_RETRE(fp,false,"File is null.");

    CREATE(network_data,1);
    NULLPO_RETRE(network_data,false,"Failed to create network.");


    CREATE(network_data->nodes,NETWORK_SIZE);
    NULLPO_RETRE(network_data->nodes,false,"Failed to create network.");

    CREATE(network_data->visitQueue,NETWORK_SIZE);
    NULLPO_RETRE(network_data->visitQueue,false,"Failed to create network.");

    CREATE(network_data->onStack,NETWORK_SIZE);
    NULLPO_RETRE(network_data->onStack,false,"Failed to create network.");

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

        switch(type){
            case 1:
                type = PROVIDER;
                break;
            case 2:
                type = PEER;
                break;
            case 3:
                type = COSTUMER;
                break;
        }


        CREATE(appendNode,1);
        if(appendNode == NULL){
            printf("Failed to create memory for tail node data %d at file line %d, skipping...",tail,i);
            continue;
        }

        *appendNode = tail;

        if(network_data->nodes[head] == NULL){
            CREATE(network_data->nodes[head],1);
            network_data->visitQueue[network_data->nodesCount] = head;
            network_data->nodes[head]->visitQueuePos = network_data->nodesCount;
            network_data->nodesCount++;
        }

        network_data->nodes[head]->links[type] = list_append(network_data->nodes[head]->links[type],(void *) appendNode);

        network_data->edgesCount++;
    }

    network_data->visitQueuePos =  network_data->edgesCount -1;
    return true;
}

void visit(int idx){
    int auxInt,auxPos;

    auxInt = network_data->visitQueue[network_data->visitQueuePos];
    auxPos = network_data->nodes[idx]->visitQueuePos;

    network_data->visitQueue[auxPos] = auxInt;
    network_data->visitQueue[network_data->visitQueuePos] = idx;

    network_data->nodes[idx]->visitQueuePos = network_data->visitQueuePos;
    network_data->nodes[auxInt]->visitQueuePos = auxPos;

    network_data->visitQueuePos--;

}

/*Returns number of direct and indirect costumers*/
void network_parse_costumers(int index){
    list_node * aux;
    int peer;

    /*Ensure node exists*/
    NULLPO_RETV(network_data->nodes[index]);

    /*Ensure it was not visited*/
    ASSERT_RETV(network_data->nodes[index]->visitQueuePos > network_data->visitQueuePos);

    /*SWITCH*/
    visit(index);

    for(aux = network_data->nodes[index]->links[COSTUMER]; aux!= NULL; aux = list_next(aux)){
        peer =  * ((int *) list_get_data(aux));
        if(network_data->onStack && network_data->onStack[peer]){
            network_data->revisitAttempts++;
#ifdef DEBUG
            printf("REVISIT ARRAY COLISION AT INDEX %d\n",index);
#endif
        }
        else if(network_data->nodes[peer]->visitQueuePos <= network_data->visitQueuePos){
            if(network_data->onStack) network_data->onStack[peer] = true;
            network_parse_costumers(peer);
            if(network_data->onStack) network_data->onStack[peer] = false;
        }

    };

    return;
}

bool network_ensure_no_costumer_cycle(){
    int peer;

    NULLPO_RETRE(network_data,false,"ERROR: network_ensure_no_costumer cycle: network is null.");

    network_data->visitQueuePos = network_data->nodesCount -1;
    network_data->revisitAttempts = 0;

    for(peer = network_data->visitQueue[0]; network_data->visitQueuePos > -1 ; peer = network_data->visitQueue[0]){

        network_data->onStack[peer] = true;
        network_parse_costumers(peer);
        network_data->onStack[peer] = false;
        
        network_data->costumerCycles += network_data->revisitAttempts;
        network_data->revisitAttempts = 0;
    }
#ifdef DEBUG
    if( network_data->costumerCycles){
        printf("DEBUG - network_ensure_no_costumer_cycle: found %d costumer cycles.\n",network_data->costumerCycles);
    }
    else  printf("DEBUG - network_ensure_no_costumer_cycle: No costumer cycles found.\n");
  
#endif
    network_data->visitQueuePos = network_data->nodesCount -1;
    return network_data->costumerCycles == 0;
}

/*
The algorithm implement here is based on the fact that if a provider can reach a node,
so can its costumers.
From here we can make "groups" of strongly connected nodes
*/
bool network_check_commercial(){
    int peer;
    list_node * ptr = NULL;

    NULLPO_RETRE(network_data,false,"Error - network_check_commercial: network is null.");
    ASSERT_RETR(network_data->costumerCycles > 0,false);

    network_data->visitQueuePos = network_data->nodesCount -1;
    network_data->revisitAttempts = 0;

    /*Go up the providers chain*/
    for(peer = network_data->visitQueue[0]; network_data->nodes[peer]->links[PROVIDER] != NULL;  
      peer =  *( (int *) list_get_data(network_data->nodes[peer]->links[PROVIDER]))){
        /*DFS on costumers in order to append them to the visited group*/
        network_parse_costumers(peer);
    }
    network_parse_costumers(peer);

    /*Now we now that all nodes on the costumer chain under this tier 1 provider are 
    *strongly connected, we just need to check for peer routes from here*/

    /*Fetch and append all direct and indirect costumers of each peer to the visited group*/
    ptr = network_data->nodes[peer]->links[PEER];
    LIST_PARSE(ptr,{
        peer =  *( (int *) list_get_data(network_data->nodes[peer]->links[PEER])) ;
        network_parse_costumers(peer);
    });

    /* - If the network is commercially connected, every node was visited by the algorithm*/
    /* - If there are still nodes to visit, it means that are nodes that the top tier provider cannot reach,
    * hence the network is not commercially connected*/

#ifdef DEBUG
    printf("DEBUG - network_check_commercial: found %d disconnected nodes\n",network_data->visitQueuePos + 1);
#endif

    if(network_data->visitQueuePos == -1)
        network_data->isCommercial = true;

    network_data->visitQueuePos = network_data->nodesCount -1;

    return network_data->isCommercial;
}

void network_update_dest_route(int nodeIndex){
    route aux_route;
    int id;
    int nHops;
    list_node * ptr;

    nHops = network_data->dest_route[nodeIndex].nHops + 1;

    aux_route.nHops = nHops;
    aux_route.advertiser = nodeIndex;
    aux_route.route_type = R_NONE;

    if(network_data->dest_route[nodeIndex].route_type >= R_COSTUMER){

        aux_route.route_type = R_COSTUMER;
        for(ptr = network_data->nodes[nodeIndex]->links[PROVIDER];ptr != NULL;ptr = list_next(ptr)){

            id = *((int *) list_get_data(ptr));

            if ( routecmp(&aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Costumer with %d hops from %d\n",id,nHops,nodeIndex);
#endif
                memcpy(&network_data->dest_route[id],&aux_route,sizeof(route));

                /*Add to visit queue*/
                network_data->visitQueuePos++;
                network_data->visitQueue[network_data->visitQueuePos] = id;
                heap_fix_up(network_data->visitQueue,network_data->visitQueuePos,routecmpById);
            }

        }

        /*UPDATE PEERS ROUTES*/ 
        aux_route.route_type = R_PEER;
        for(ptr = network_data->nodes[nodeIndex]->links[PEER];ptr != NULL;ptr = list_next(ptr)){
            id = *((int *) list_get_data(ptr));

            if ( routecmp(&aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Peer with %d hops from %d\n",id,nHops,nodeIndex);
#endif
                memcpy(&network_data->dest_route[id],&aux_route,sizeof(route));

                /*Add to visit queue*/
                network_data->visitQueuePos++;
                network_data->visitQueue[network_data->visitQueuePos] = id;
                heap_fix_up(network_data->visitQueue,network_data->visitQueuePos,routecmpById);
            }

        }
    }

    if(!network_data->isCommercial || network_data->flag != CALC_TYPE){
        /*Only needed if the network is not commercially connected , see (1)*/
        /*Or user choose to calculate number of hops/ advertisers aswell*/
        aux_route.route_type = R_PROVIDER;

        for(ptr = network_data->nodes[nodeIndex]->links[COSTUMER];ptr != NULL;ptr = list_next(ptr)){
            id = *((int *) list_get_data(ptr));

            if ( routecmp(&aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Provider with %d hops from %d\n",id,nHops,nodeIndex);
#endif
                memcpy(&network_data->dest_route[id],&aux_route,sizeof(route));

                /*Add to visit queue*/
                network_data->visitQueuePos++;
                network_data->visitQueue[network_data->visitQueuePos] = id;
                heap_fix_up(network_data->visitQueue,network_data->visitQueuePos,routecmpById);
            }
        }
    }

}


void network_find_paths_to(int destination, enum calc_type flag){
    int aux;

    NULLPO_RETVE(network_data,"Error - network_find_paths_to : network is null.");
    ASSERT_RETVE(destination >= NETWORK_SIZE,"Error - network_find_paths_to : destination not found in network.");
    NULLPO_RETVE(network_data->nodes[destination],"Error - network_find_paths_to : destination not found in network.");

    if(network_data->dest_route == NULL){
        CREATE(network_data->dest_route,NETWORK_SIZE);
    }
    else{
        memset(network_data->dest_route,0,NETWORK_SIZE * sizeof(route));
    }
    network_data->dest = destination;

    if(network_data->isCommercial &&  flag == CALC_TYPE){
        /*A network being commercially connected implies that each node, in the worst case scenario,
        * can connect to all the othes via a provider route, by initiliazing all routes type to "provider",
        * we can skip iterating over all costumers routes later on . (1) */
        FOREACH(NETWORK_SIZE,network_data->dest_route[iterator].route_type = R_PROVIDER );
    }

    memset(network_data->visitQueue,0,sizeof(int) * NETWORK_SIZE);
    network_data->visitQueue[0] = destination;
    network_data->visitQueuePos = 0;
    

    network_data->dest_route[destination].route_type = R_SELF;
    network_data->dest_route[destination].nHops = 0;
    network_data->dest_route[destination].advertiser = destination;

    network_data->flag = flag;

    while(network_data->visitQueuePos > -1){
        network_update_dest_route(network_data->visitQueue[0]);
        aux = network_data->visitQueue[0];
        network_data->visitQueue[0] = network_data->visitQueue[network_data->visitQueuePos];
        network_data->visitQueue[network_data->visitQueuePos] = aux;
        heap_fix_down(network_data->visitQueue,0,network_data->visitQueuePos,routecmpById);
        network_data->visitQueuePos--;
    }
    
    printf("network_find_paths_to: Finishing calculating routes to %d.\n",destination);
}

void free_link(int * rt){
    free(rt);
}

void network_destroy(){
    NULLPO_RETV(network_data);

    if(network_data->nodes != NULL){
        FOREACH(NETWORK_SIZE,{
            if(network_data->nodes[iterator] == NULL) continue;

            list_free(network_data->nodes[iterator]->links[PROVIDER], (void *) free_link);
            list_free(network_data->nodes[iterator]->links[COSTUMER],(void *) free_link);
            list_free(network_data->nodes[iterator]->links[PEER],(void *) free_link);
            
            free(network_data->nodes[iterator]);
            network_data->nodes[iterator] = NULL;
        });
    }

    if(network_data->dest_route){
        free(network_data->dest_route);
        network_data->dest_route = NULL;
    }

    if(network_data->visitQueue){
        free(network_data->visitQueue);
        network_data->visitQueue = NULL;
    }
    
    if(network_data->onStack){
        free(network_data->onStack);
        network_data->onStack = NULL;
    }
    free(network_data);
}

void network_print_log(FILE * fp){

    char routetype[][50] = {
        "has no route to destination",
        "elects a provider route",
        "elects a peer route",
        "elects a costumer route",
        "is the destination itself"
    };

    NULLPO_RETVE(network_data,"Network is null");
    NULLPO_RETVE(fp,"File descriptor is null");

    fprintf(fp,"Network Info:\n");
    fprintf(fp,"\t%d Nodes, %d Edges\n",network_data->nodesCount,network_data->edgesCount); 

    if(network_data->costumerCycles){
        fprintf(fp,"\t%d costumer cycles found.\n",network_data->costumerCycles);    
        return;  
    }  
    else fprintf(fp,"\tNo costumer cycles found.\n");
    
    if(network_data->isCommercial){
        fprintf(fp,"\tCommercially connected.\n");      
    }
    else{
        fprintf(fp,"\tNot commercially connected.\n");      
    }

    if(network_data->dest){
        fprintf(fp,"\n");
        fprintf(fp,"Route types elected to reach node %d:\n",network_data->dest);

        for(int iterator = 0; iterator < NETWORK_SIZE; iterator++){
            if(network_data->nodes[iterator] != NULL){
                fprintf(fp,"\t Node %d",iterator);

                if(network_data->flag & CALC_TYPE){
                    fprintf(fp," %s",routetype[network_data->dest_route[iterator].route_type]);
                }

                if(network_data->dest_route[iterator].route_type != R_SELF 
                    && network_data->dest_route[iterator].route_type != R_NONE){
                    
                    if(network_data->flag & CALC_ADVERTISER){
                        fprintf(fp," from %d",network_data->dest_route[iterator].advertiser); 
                    }
                    if(network_data->flag & CALC_HOPS){
                        fprintf(fp," with %d hops",network_data->dest_route[iterator].nHops); 
                    }
                }
                fprintf(fp,"\n");
                   
            }
        }
    }


}

struct network_interface * network_init(){
    struct network_interface * network = NULL;

    CREATE(network,1);
    network->create_from_file = network_create_from_file;
    network->ensure_no_costumer_cycle = network_ensure_no_costumer_cycle;
    network->check_commercial = network_check_commercial;
    network->find_paths_to = network_find_paths_to;;
    network->print_log = network_print_log;
    network->destroy = network_destroy;

    return network;
}