#include "defs.h"
#include "list.h"


struct node_{
    void * data;
    struct node_ * next;
}

list_node * list_append(list_node * ptr, void * data){
    list_node * retval;

    NULLPO_RETRE(ptr,NULL,"LIST - Cannot append a node with empty data!");

    CREATE(retval,1);
    NULLPO_RETR(retval,NULL);

    retval->next = ptr;

    retval->data = data;

    return retval;
}