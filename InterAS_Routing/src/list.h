#ifndef LIST_HEADER
#define LIST_HEADER

struct node_;
typedef struct node_ list_node;

void * list_get_data(list_node * ptr);
list_node * list_append(list_node * ptr, void * data);
list_node * list_next(list_node * ptr);
list_node * list_free_node(list_node * prev,list_node * rmn,void (*free_data)(void*));
void list_free(list_node * head, void (* free_data) (void*));

#define LIST_PARSE(L,O) \
    while(L != NULL){ \
        O; \
        L = list_next(L); \
    }

#endif

