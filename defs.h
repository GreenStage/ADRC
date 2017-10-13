#ifndef DEFINES_HEADER
#define DEFINES_HEADER

/*****Debug mode switch****/
/*#define DEBUG*/

/******************************************************
 ********************* Values**************************
 *****************************************************/
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_MISSING_ARG 2

#define SMALL_STR_SIZE 50
#define MEDIUM_STR_SIZE 100

#define EMPTY_HOP 0
#define MAX_TREE_HEIGHT 16

/******************************************************
 ***************** String macros **********************
 *****************************************************/

#define PRINT_STR(A) printf("%s\n",A);

/*Returns 1 in case the string is empty or null*/
#define EMPTY_STRING(A) ({ (A == NULL || strlen(A) < 1) ?  1 : 0;})

/******************************************************
 ***************** Array macros  **********************
 *****************************************************/

/*Retrieves an array length*/
#define ARRAY_LENGTH(t) ( sizeof(t) / sizeof(t[0]) )

/*Iterates and calls O for each iteration*/
#define FOREACH(NUM_IT,O) { \
    int iterator; \
    for(iterator = 0; iterator < (int) NUM_IT; iterator++){ \
        O; \
    } \
}

/******************************************************
 ************ Variable ensure macros ******************
 *****************************************************/

/*Returns void if the condition t is verified*/
#define ASSERT_RETV(t) \
    do { if (t) return; } while(0)

/*Returns variable v if the condition t is verified*/
#define ASSERT_RETR(t,v) \
    do { if(t) return v; } while(0)

/*Returns void and prints a message if the condition t is verified*/    
#define ASSERT_RETVE(t,v) \
    do {  \
        if (t) { \
            printf("%s\n", (char * ) v); \
            return; \
        } \
    } while(0)

/*Returns variable r and prints a message if the condition t is verified*/    
#define ASSERT_RETRE(t,r,m) \
do {  \
    if (t) { \
        printf("%s\n", (char * ) m); \
        return r; \
    } \
} while(0)

/*Extended macros to ensure that a variable is not null*/
#define NULLPO_RETV(t) ASSERT_RETV(t == NULL)
#define NULLPO_RETR(t,r) ASSERT_RETR(t == NULL,r)
#define NULLPO_RETVE(t,v) ASSERT_RETVE(t == NULL,v)
#define NULLPO_RETRE(t,r,m) ASSERT_RETRE(t == NULL,r,m)





/******************************************************
 ***************** Memory macros  *********************
 *****************************************************/

/*Allocs and initializes memmory*/
#define CREATE(ptr,n) \
do{ \
    ptr =  malloc(n * sizeof(__typeof__(*ptr))); \
    if(ptr == NULL){ \
        printf("Cannot create memmory: line %d in %s\n",__LINE__,__func__); \
    } \
    else{ \
        memset(ptr,0,n * sizeof(__typeof__(*ptr))); \
    } \
} while(0)


#endif