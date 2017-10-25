
#include "defs.h"
#include <string.h>
#include "list.h"


struct node_{
    void * data;
    struct node_ * next;
};

void * list_get_data(list_node * ptr){
    return ptr->data;
}

list_node * list_next(list_node * ptr){
    return ptr->next;
}

void list_free(list_node * head, void (* free_data) (void*)){
    list_node * aux = head;
    while(head != NULL){
        aux = head;
        head = head->next;
        if(aux->data != NULL){
            free_data(aux->data);
        }

        free(aux);
    }
}
list_node * list_append(list_node * ptr, void * data){
    list_node * retval;

    NULLPO_RETRE(data,NULL,"ERROR - list_append: Cannot append a node with empty data!");

    CREATE(retval,1);
    NULLPO_RETR(retval,NULL);

    retval->next = ptr;

    retval->data = data;

    return retval;
}