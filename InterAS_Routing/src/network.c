#include <string.h>
#include "defs.h"
#include "list.h"
#include "network.h"
#include "heap.h"
#include <time.h>

enum link_type {
    COSTUMER = 0,
    PEER,
    PROVIDER
};
enum link_type reverseLink[]={
    PROVIDER,
    PEER,
    COSTUMER
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
    int adjCtr;
    bool stubBy1Provider;
    int topTierId;
    unsigned visitQueuePos; /* FOR FAST LOOKUP ON THE VISIT QUEUE ARRAY*/
} node;

typedef struct route_{
    enum route_type route_type;
    unsigned nHops; 
    unsigned advertiser;
}route;

typedef struct graph_{
    /*Adjacency list instance, representing the graph */
    node * nodes[NETWORK_SIZE];

    /*Array of routes to a destination*/
    route dest_route[NETWORK_SIZE];
    
    list_node * top_providers;
    unsigned top_provider_ctr;

    bool onStack[NETWORK_SIZE];

    /*Array keeping track of unvisited nodes*/
    /*Visited nodes stack on the end of the array, separated from the 
     *unvisited ones by visitQueuePos*/
    /*It is treated like a heap on statistic calculation*/
    /*Should have plenty of space for repeated visiting intents*/
    unsigned visitQueue[3 * NETWORK_SIZE];

    /* "To visit" / "Visited" divider*/
    long int visitQueuePos;

    /*Graph information*/
    bool hasCostumerCycles,isCommercial;

    unsigned costumerPaths, peerPaths, providerPaths;
    unsigned nodesCount, edgesCount;
    enum calc_type flag;

    enum calc_type stats;
    unsigned dest;
    unsigned distanceArray[MAX_DISTANCE];
    long unsigned routeTypesArray[R_SELF+1];

} graph;

graph * network_data = NULL;


void free_link(unsigned * rt){
    free(rt);
}

bool routecmp(route * r1,route * r2){
    if( ( r1->route_type == r2->route_type && r1->nHops < r2->nHops ) 
        || r1->route_type > r2->route_type || r2->nHops == -1) {
            
        return true;
    }
    else return false;
} 

bool routecmpById(unsigned idx1,unsigned idx2){
    return routecmp(&network_data->dest_route[idx1],&network_data->dest_route[idx2]);
} 

bool network_create_from_file(FILE * fp){
    char line[100];
    unsigned i,head, tail , type;

    unsigned * appendNode;

    NULLPO_RETRE(fp,false,"File is null.");

    CREATE(network_data,1);
    NULLPO_RETRE(network_data,false,"Failed to create network.");

    i = 0;
    while( fgets(line,100,fp) ){
        i++;
        
        if( sscanf(line,"%d %d %d",&tail,&head,&type) < 3 ){
            printf("Error reading file line %d, skipping...",i);
            continue;
        }

        if( type > 3 ||  tail >= NETWORK_SIZE || head >= NETWORK_SIZE || head < 1 || tail < 1){
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
        network_data->nodes[head]->adjCtr++;

        if(network_data->nodes[head]->adjCtr > 1){
            network_data->nodes[head]->stubBy1Provider = false;
        }
        else if(type == PROVIDER) network_data->nodes[head]->stubBy1Provider = true;

        if(type == PROVIDER){
            network_data->nodes[head]->topTierId = -1;
        }

        network_data->edgesCount++;
    }

    for(i = 0, type = 0; i < NETWORK_SIZE;i ++){
        if(network_data->nodes[i] && network_data->nodes[i]->stubBy1Provider){
            type++;
        }
        if(network_data->nodes[i] && network_data->nodes[i]->topTierId != -1){
            CREATE(appendNode,1);
            *appendNode = i;
            network_data->top_providers = list_append(network_data->top_providers,(void *) appendNode);
            network_data->nodes[i]->topTierId= network_data->top_provider_ctr;
            network_data->top_provider_ctr++;
        }
    }
    printf("Found %d (%f%%) stub nodes with only 1 neighbor (Provider)\n",type,(float)(((float)type/(float)network_data->nodesCount)*100));

    network_data->visitQueuePos =  network_data->edgesCount -1;

    return true;
}

void visit(unsigned idx){
    unsigned auxInt = 0,auxPos = 0;

    auxInt = network_data->visitQueue[network_data->visitQueuePos];
    auxPos = network_data->nodes[idx]->visitQueuePos;

    network_data->visitQueue[auxPos] = auxInt;
    network_data->visitQueue[network_data->visitQueuePos] = idx;

    network_data->nodes[idx]->visitQueuePos = network_data->visitQueuePos;
    network_data->nodes[auxInt]->visitQueuePos = auxPos;

    network_data->visitQueuePos--;

}

/*Returns number of direct and indirect connected nodes*/
void network_parse_connected(unsigned index,enum link_type type){
    list_node * aux;
    unsigned peer;

    /*Ensure node exists*/
    NULLPO_RETV(network_data->nodes[index]);

    /*Ensure it was not visited*/
    ASSERT_RETV(network_data->nodes[index]->visitQueuePos > network_data->visitQueuePos);

    /*SWITCH*/
    visit(index);

    for(aux = network_data->nodes[index]->links[type]; aux!= NULL; aux = list_next(aux)){
        peer =  * ((unsigned *) list_get_data(aux));
        if(network_data->onStack[peer]){
            network_data->hasCostumerCycles = true;
#ifdef DEBUG
            printf("Revisit attempt at index %d\n",index);
#endif
        }
        else if(network_data->nodes[peer]->visitQueuePos <= network_data->visitQueuePos){
            network_data->onStack[peer] = true;
            network_parse_connected(peer,type);
            network_data->onStack[peer] = false;
        }

    };

    return;
}

bool network_ensure_no_costumer_cycle(){
    unsigned peer;

    NULLPO_RETRE(network_data,false,"ERROR: network_ensure_no_costumer cycle: network is null.");

    network_data->visitQueuePos = network_data->nodesCount -1;
    network_data->hasCostumerCycles = false;

    for(peer = network_data->visitQueue[0]; network_data->visitQueuePos > -1 ; peer = network_data->visitQueue[0]){

        network_data->onStack[peer] = true;
        network_parse_connected(peer,COSTUMER);
        network_data->onStack[peer] = false;

        if(network_data->hasCostumerCycles) break;
    }
#ifdef DEBUG
    if( network_data->hasCostumerCycles){
        printf("DEBUG - network_ensure_no_costumer_cycle: found costumer cycles.\n");
    }
    else  printf("DEBUG - network_ensure_no_costumer_cycle: No costumer cycles found.\n");
  
#endif
    network_data->visitQueuePos = network_data->nodesCount -1;
    return !network_data->hasCostumerCycles;
}

/*
The algorithm implement here is based on the fact that if a provider can reach a node,
so can its costumers.
From here we can make "groups" of strongly connected nodes
*/
bool network_check_commercial(){
    unsigned id1,id2;
    unsigned * appendNode;
    bool ** topTierPeersMatrix;
    unsigned * topTierPositions;

    list_node * ptr1, * ptr2;

    NULLPO_RETRE(network_data,false,"Error - network_check_commercial: network is null.");
    ASSERT_RETR(network_data->hasCostumerCycles,false);
    NULLPO_RETR(network_data->top_providers,false);

    CREATE(topTierPositions,network_data->top_provider_ctr);
    CREATE(topTierPeersMatrix,network_data->top_provider_ctr);
    FOREACH(network_data->top_provider_ctr,CREATE(topTierPeersMatrix[iterator],network_data->top_provider_ctr));

    network_data->visitQueuePos = network_data->nodesCount -1;

    for(ptr1 = network_data->top_providers; ptr1 != NULL; ptr1 = list_next(ptr1)){
        id1 =  *( (unsigned *) list_get_data(ptr1));
        topTierPositions[network_data->nodes[id1]->topTierId] = id1;
        for(ptr2 = network_data->nodes[id1]->links[PEER]; ptr2 != NULL; ptr2 = list_next(ptr2)){
            id2 =  *( (unsigned *) list_get_data(ptr2));
            if(network_data->nodes[id2]->topTierId >= 0){
                topTierPeersMatrix[network_data->nodes[id1]->topTierId][network_data->nodes[id2]->topTierId] = true;
            }
        }
    }   

    network_data->isCommercial = true;
    for(id1 = 0; id1 < network_data->top_provider_ctr;id1++){
        for(id2 = 0; id2 < network_data->top_provider_ctr;id2++){
            if(id1 == id2) continue;
            if(topTierPeersMatrix[id1][id2] != true){
                network_data->isCommercial = false;
                break;
            }
        }       
    }
  
    if(!network_data->isCommercial){
        char response;
        printf("network_check_commercial: network is not commercially connected\n" );
        printf("Do you wish to make the network commercially connected (create missing peer links between tier 1 providers)?\n");
        printf("(y) / (n)\n");
        while(1){
            if(scanf("%c",&response)){
                if(response == 'y'){
                    for(id1 = 0; id1 < network_data->top_provider_ctr;id1++){
                        for(id2 = 0; id2 < network_data->top_provider_ctr;id2++){
                            if(id1 == id2) continue;
                            if(topTierPeersMatrix[id1][id2] != true){
                                CREATE(appendNode,1);
                                *appendNode = topTierPositions[id2];
                                network_data->nodes[topTierPositions[id1]]->links[PEER] = list_append(network_data->nodes[topTierPositions[id1]]->links[PEER],(void *) appendNode);
                                network_data->edgesCount++;
                            }
                        }       
                    }
                    network_data->isCommercial = network_check_commercial();

                    break;
             }
                else if(response == 'n'){
                    break;
                }
            }
        }
    }


    FOREACH(network_data->top_provider_ctr,free(topTierPeersMatrix[iterator]));
    free(topTierPeersMatrix);
    free(topTierPositions);

    network_data->visitQueuePos = network_data->nodesCount -1;
    return network_data->isCommercial;
}

void network_update_dest_route(unsigned nodeIndex){
    static route * aux_route = 0; 
    static unsigned id = 0;
    static list_node * ptr = NULL;


    aux_route = &network_data->dest_route[0];

    network_data->distanceArray[network_data->dest_route[nodeIndex].nHops]++;
    aux_route->nHops = network_data->dest_route[nodeIndex].nHops + 1;

    aux_route->advertiser = nodeIndex;
    aux_route->route_type = R_NONE;

    /*Adversite costumer routes to all neighbors*/
    if(network_data->dest_route[nodeIndex].route_type >= R_COSTUMER){

        aux_route->route_type = R_COSTUMER;
        for(ptr = network_data->nodes[nodeIndex]->links[PROVIDER];ptr != NULL;ptr = list_next(ptr)){

            id = *((unsigned *) list_get_data(ptr));

            if ( routecmp(aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Costumer with %d hops from %d\n",id,aux_route->nHops,nodeIndex);
#endif
                network_data->routeTypesArray[network_data->dest_route[id].route_type]--;
                network_data->routeTypesArray[R_COSTUMER]++;


                memcpy(&network_data->dest_route[id],aux_route,sizeof(route));

                
                /*Add to visit queue*/
                network_data->visitQueuePos++;
                network_data->visitQueue[network_data->visitQueuePos] = id;

                heap_fix_up(network_data->visitQueue,network_data->visitQueuePos,routecmpById);
            }

        }

        /*UPDATE PEERS ROUTES*/ 
        aux_route->route_type = R_PEER;
        for(ptr = network_data->nodes[nodeIndex]->links[PEER];ptr != NULL;ptr = list_next(ptr)){
            id = *((unsigned *) list_get_data(ptr));

            if ( routecmp(aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Peer with %d hops from %d\n",id,aux_route->nHops,nodeIndex);
#endif
                network_data->routeTypesArray[network_data->dest_route[id].route_type]--;
                network_data->routeTypesArray[R_PEER]++;

                memcpy(&network_data->dest_route[id],aux_route,sizeof(route));

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
        aux_route->route_type = R_PROVIDER;

        for(ptr = network_data->nodes[nodeIndex]->links[COSTUMER];ptr != NULL;ptr = list_next(ptr)){
            id = *((unsigned *) list_get_data(ptr));

            if ( routecmp(aux_route,&network_data->dest_route[id]) || network_data->dest_route[id].advertiser == nodeIndex){
#ifdef DEBUG
                printf("Node %d route updated to Provider with %d hops from %d\n",id,aux_route->nHops,nodeIndex);
#endif
                network_data->routeTypesArray[network_data->dest_route[id].route_type]--;
                network_data->routeTypesArray[R_PROVIDER]++;

                memcpy(&network_data->dest_route[id],aux_route,sizeof(route));

                /*Add to visit queue*/
                network_data->visitQueuePos++;
                network_data->visitQueue[network_data->visitQueuePos] = id;
                
                heap_fix_up(network_data->visitQueue,network_data->visitQueuePos,routecmpById);
            }
        }
    }

}


void network_find_paths_to(unsigned destination, enum calc_type flag){
    static unsigned it = 0,aux = 0;

    NULLPO_RETVE(network_data,"Error - network_find_paths_to : network is null.");
    ASSERT_RETVE(destination >= NETWORK_SIZE,"Error - network_find_paths_to : destination not found in network.");
    NULLPO_RETVE(network_data->nodes[destination],"Error - network_find_paths_to : destination not found in network.");

    memset(network_data->dest_route,0,NETWORK_SIZE * sizeof(route));
    memset(network_data->distanceArray,0,sizeof(unsigned) * MAX_DISTANCE);

    memset(network_data->routeTypesArray,0, (R_SELF +1) * sizeof(long unsigned));

    network_data->dest = destination;

    if(network_data->isCommercial &&  flag & CALC_TYPE){
        /*A network being commercially connected implies that each node, in the worst case scenario,
        * can connect to all the othes via a provider route, by initiliazing all routes type to "provider",
        * we can skip iterating over all costumers routes later on . (1) */
        for(it = 0; it < NETWORK_SIZE;it++){
            network_data->dest_route[it].route_type = R_PROVIDER;
            network_data->dest_route[it].nHops = -1;
        }
        network_data->routeTypesArray[R_PROVIDER] = network_data->nodesCount - 1;
    }

    /*memset(network_data->visitQueue,0,sizeof(unsigned) * NETWORK_SIZE);*/
    network_data->visitQueue[0] = destination;
    network_data->visitQueuePos = 0;
    

    network_data->dest_route[destination].route_type = R_SELF;
    network_data->dest_route[destination].nHops = 0;
    network_data->dest_route[destination].advertiser = destination;

    network_data->flag = flag;

    while(network_data->visitQueuePos > -1){
        it = network_data->visitQueue[0];
        network_update_dest_route(it);

        aux = network_data->visitQueue[0];
        network_data->visitQueue[0] = network_data->visitQueue[network_data->visitQueuePos];
        network_data->visitQueue[network_data->visitQueuePos] = aux;
        heap_fix_down(network_data->visitQueue,0,network_data->visitQueuePos,routecmpById);
        network_data->visitQueuePos--;
    }

}

void network_parse_all(enum calc_type type){
    route aux[NETWORK_SIZE];
    long unsigned auxTypeArray[R_SELF+1];
    int sumDistanceArray[MAX_DISTANCE];
    list_node * listptr;
    unsigned i,id,j,k=0;
    clock_t start, end;
#ifndef DEBUG
    unsigned step = network_data->nodesCount / 50 + 1;
#endif
    memset(sumDistanceArray,0,sizeof(unsigned) * MAX_DISTANCE);
    memset(auxTypeArray,0,sizeof(long unsigned) * (R_SELF +1));
    memset(network_data->routeTypesArray,0, (R_SELF +1) * sizeof(long unsigned));

    start = clock();

    printf("Calculating statistics,this might take some minutes.\n");
    

    for(i = 1,k=0; i < NETWORK_SIZE; i++){
#ifndef DEBUG
      if(i%step==0){
            end = clock();
            int c = (double) (end - start)/CLOCKS_PER_SEC;
            printf("[%d%%] elapsed time: %d:%d minutes..",k*100/network_data->nodesCount,(int)c/60,(int)c%60);
            fflush(stdout);
            printf("\r");
        }
#endif
        if(network_data->nodes[i] != NULL){
            /*Heuristic*/
            if(network_data->nodes[i]->stubBy1Provider || (network_data->isCommercial && network_data->nodes[i]->adjCtr == 1))
                continue;

            network_find_paths_to(i,type);
            k++;
            /*Heuristic*/
            for(listptr = network_data->nodes[i]->links[COSTUMER]; listptr != NULL; listptr = list_next(listptr)){
                id = *((unsigned *) list_get_data(listptr));

                if(!network_data->nodes[id]->stubBy1Provider)
                    continue;
                
                    k++;
                auxTypeArray[R_COSTUMER] = auxTypeArray[R_COSTUMER] +  network_data->routeTypesArray[R_COSTUMER] + 1;
                auxTypeArray[R_PROVIDER] = auxTypeArray[R_PROVIDER] +  network_data->routeTypesArray[R_PROVIDER] - 1;
                auxTypeArray[R_PEER]     = auxTypeArray[R_PEER]     +  network_data->routeTypesArray[R_PEER];

                if(type & CALC_HOPS){
                    sumDistanceArray[1] = sumDistanceArray[1] + 1;
                    sumDistanceArray[2] = sumDistanceArray[2] + network_data->distanceArray[1] -1;
                    for(j = 2; j < MAX_DISTANCE - 1;j++){
                        sumDistanceArray[j + 1]  = sumDistanceArray[j + 1] + network_data->distanceArray[j];
                    }     
                }
  
            }
            
            /*Heuristic*/
            if(network_data->isCommercial){
                for(listptr = network_data->nodes[i]->links[PEER]; listptr != NULL; listptr = list_next(listptr)){
                    id = *((unsigned *) list_get_data(listptr));

                    if(network_data->nodes[id]->adjCtr != 1)
                        continue;
                        k++;
                    auxTypeArray[R_COSTUMER] = auxTypeArray[R_COSTUMER] +  network_data->routeTypesArray[R_COSTUMER];
                    auxTypeArray[R_PROVIDER] = auxTypeArray[R_PROVIDER] +  network_data->routeTypesArray[R_PROVIDER];
                    auxTypeArray[R_PEER]     = auxTypeArray[R_PEER]     +  network_data->routeTypesArray[R_PEER];

                    if(type & CALC_HOPS){
                        sumDistanceArray[1] = sumDistanceArray[1] + 1;
                        sumDistanceArray[2] = sumDistanceArray[2] + network_data->distanceArray[1] -1;
    
                        for(j = 2; j < MAX_DISTANCE - 1;j++){
                            sumDistanceArray[j + 1]  = sumDistanceArray[j + 1] + network_data->distanceArray[j];
                        }  
                    }
                }

                for(listptr = network_data->nodes[i]->links[PROVIDER]; listptr != NULL; listptr = list_next(listptr)){
                    id = *((unsigned *) list_get_data(listptr));

                    if(network_data->nodes[id]->adjCtr != 1)
                        continue;
                        k++;
                    auxTypeArray[R_COSTUMER] = auxTypeArray[COSTUMER]   +  network_data->routeTypesArray[R_COSTUMER] - 1;
                    auxTypeArray[R_PROVIDER] = auxTypeArray[R_PROVIDER] +  network_data->routeTypesArray[R_PROVIDER] + 1;
                    auxTypeArray[R_PEER]     = auxTypeArray[R_PEER]     +  network_data->routeTypesArray[R_PEER];     

                    if(type & CALC_HOPS){
                        sumDistanceArray[1] = sumDistanceArray[1] + 1;
                        sumDistanceArray[2] = sumDistanceArray[2] + network_data->distanceArray[1] -1;
    
                        for(j = 2; j < MAX_DISTANCE - 1;j++){
                            sumDistanceArray[j + 1]  = sumDistanceArray[j + 1] + network_data->distanceArray[j];
                        } 
                    }                         
                }
            }

            for(j = R_NONE;j <= R_SELF;j++){
                auxTypeArray[j] =  network_data->routeTypesArray[j] + auxTypeArray[j];
            }

            for(j = 0; j < MAX_DISTANCE;j++){
                sumDistanceArray[j] = sumDistanceArray[j] + network_data->distanceArray[j];
            }

            
        }

    }

    for(j = 0; j < MAX_DISTANCE;j++){
        network_data->distanceArray[j] = sumDistanceArray[j];
    }

    for(j = R_NONE;j <= R_SELF;j++){
        network_data->routeTypesArray[j] = auxTypeArray[j];
    }

    memcpy(network_data->dest_route,aux,NETWORK_SIZE*sizeof(route));
    network_data->stats = type;
    network_data->dest = false;

    end = clock();
    int c = (double) (end - start)/CLOCKS_PER_SEC;

    printf("Elapsed time: %d minutes and %d seconds!\n",(int)c/60,(int)c%60);

    return;
}

void network_print_log(FILE * fp){
    unsigned i;
    char routetype[][50] = {
        "has no route to destination",
        "elects a provider route",
        "elects a peer route",
        "elects a costumer route",
        "is the destination itself"
    };
    char routeAll[][50] = {
        "provider routes elected",
        "peer routes elected",
        "costumer routes elected"
    };

    NULLPO_RETVE(network_data,"Network is null");
    NULLPO_RETVE(fp,"File descriptor is null");

    fprintf(fp,"Network Info:\n");
    fprintf(fp,"\t%d Nodes, %d Edges\n",network_data->nodesCount,network_data->edgesCount); 

    if(network_data->hasCostumerCycles){
        fprintf(fp,"\tNetwork has costumer cycles.\n");    
        return;  
    } 
    else fprintf(fp,"\tNo costumer cycles found.\n");
    
    if(network_data->isCommercial){
        fprintf(fp,"\tCommercially connected.\n");      
    }
    else{
        fprintf(fp,"\tNot commercially connected.\n");      
    }

    if(network_data->stats & CALC_TYPE){
        long unsigned sum = network_data->routeTypesArray[R_COSTUMER] + network_data->routeTypesArray[R_PEER]
                            + network_data->routeTypesArray[R_PROVIDER];
        fprintf(fp,"\nRoutes type stats:\n");
        for(i = R_PROVIDER;i <R_SELF;i++){
            long double pc = network_data->routeTypesArray[i];
            pc = pc / sum * 100;
            fprintf(fp,"There are %lu [%02Lf%%] %s\n",network_data->routeTypesArray[i],pc, routeAll[i - R_PROVIDER]);
        }
    }
    if(network_data->stats & CALC_HOPS){
        fprintf(fp,"\nThere are : \n");
        for(i = 1; i < MAX_DISTANCE; i++){
            if(network_data->distanceArray[i] != 0){
                fprintf(fp,"\t %d paths with %d hops\n",network_data->distanceArray[i],i);
            }
        }
    }

    if(network_data->dest){
        unsigned iterator;
        fprintf(fp,"\n");
        fprintf(fp,"Route types elected to reach node %d:\n",network_data->dest);

        for(iterator = 0; iterator < NETWORK_SIZE; iterator++){
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
                else if(network_data->dest_route[iterator].route_type == R_SELF){
                    fprintf(fp," is the destination itself");
                }
                else if(network_data->dest_route[iterator].route_type == R_NONE){
                    fprintf(fp," has no route");
                }
                fprintf(fp,"\n");
                   
            }
        }
    }
    printf("Log file generated in /logs\n");
}

void network_destroy(){
    unsigned iterator;
    NULLPO_RETV(network_data);

    for( iterator= 0; iterator < NETWORK_SIZE; iterator++){
        if(network_data->nodes[iterator] == NULL) continue;
        
        list_free(network_data->nodes[iterator]->links[PROVIDER], (void *) free_link);
        list_free(network_data->nodes[iterator]->links[COSTUMER],(void *) free_link);
        list_free(network_data->nodes[iterator]->links[PEER],(void *) free_link);
        
        free(network_data->nodes[iterator]);
        network_data->nodes[iterator] = NULL;
        
    }

    list_free(network_data->top_providers,(void *) free_link);
    free(network_data);
}


void network_init(){
    CREATE(network,1);
    network->create_from_file = network_create_from_file;
    network->ensure_no_costumer_cycle = network_ensure_no_costumer_cycle;
    network->check_commercial = network_check_commercial;
    network->find_paths_to = network_find_paths_to;
    network->parse_all = network_parse_all;
    network->print_log = network_print_log;
    network->destroy = network_destroy;
}

void network_finalize(){
    free(network);
}
