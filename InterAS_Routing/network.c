#include "defs.h"
#include "list.h"
#include "network.h"

struct graph_{
    list_node ** adjList;
   /* int headIds[NETWORK_SIZE];*/
}

enum link_type {
    PROVIDER = 1,
    COSTUMER,
    PEER
}

typedef struct link_{
    int peer;
    enum link_type type;

}link;
/*
int hash_value(int value){
    value = ((value >> 16) ^ value) * 0x45d9f3b;
    value = ((value >> 16) ^ value) * 0x45d9f3b;
    value = (value >> 16) ^ value;
    return value % NETWORK_SIZE;
}

int get_adjList_pos(graph * network, int value){
    int currentId;
    int count = 0;
    int hashedIdx = hash_value(value);

    currentId = network->headIds[hashedIdx];

    while(currentId != 0 && currentId != value){
        ASSERT_RETRE(count >= NETWORK_SIZE, -1, "No free positions found.");

        hashedIdx = ( hashedIdx + 1) % NETWORK_SIZE;
        currentId = network->headIds[hashedIdx];
        count ++;

    }
    return hashedIdx;
}   
*/

graph * network_create_from_file(FILE * fp){
    graph * network;
    char line[100];
    int i, idx, head, tail , type;
    link * appendNode;

    NULLPO_RETRE(fp,NULL,"File is null.");
    


    CREATE(network,1);
    NULLPO_RETRE(network,NULL,"Failed to create network.");

    
    CREATE(network->adjList,NETWORK_SIZE);
    NULLPO_RETRE(network,network,"Failed to create network.");

    memset(network->headIds,0,NETWORK_SIZE);

    i = 0;
    while( fgets(line,100,fp) ){
        i++;
        
        if( sscanf(line,"%d %d %d",&tail,&head,&type) < 3 ){
            printf("Error reading file line %d, skipping...",i);
            continue;
        }

        if(tail < 0  || head < 0|| tail >= NETWORK_SIZE || head >= NETWORK_SIZE){
            printf("Invalid node provided at line %d, skipping...",i);
            continue;
        }
       
 /*       network->headIds[idx] = head;
*/
        CREATE(append_node,1);
        if(append_node == NULL){
            printf("Failed to create memory for tail node data %d at file line %d, skipping...",tail,i);
            continue;
        }

        append_node->peer = tail;
        append_node->type = type;
         
        network->adjList[tail] = list_append(network->adjList[head],(void *) append_node);

        append_node = NULL;
/*
        ASSERT_RETRE( (idx = get_adjList_pos(network,tail) )  == -1 , network, "Network structure is full." );
      
        network->headIds[idx] = tail;
  */
        CREATE(append_node,1);
        if(append_node == NULL){
            printf("Failed to create memory for head node data %d at file line %d, skipping...",head,i);
            continue;
        }

        append_node->peer = head;
        append_node->type = type == 3 ? type : 3 - type;

        network->adjList[tail] = list_append(network->adjList[tail],(void *) append_node);       
    }

    return network;

}