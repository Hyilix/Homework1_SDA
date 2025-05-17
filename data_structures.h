/* URSESCU Sebastian - 315CA */
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <errno.h>
#include <stdio.h>

// simple DIE function, used for sanity checks
#define DIE(assertion, call_description)  \
    do                                    \
    {                                     \
        if (assertion)                    \
        {                                 \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__);  \
            perror(call_description);     \
            exit(errno);                  \
        }                                 \
    } while (0)

#define MIN(x, y) ((x) < (y) ? (x) : (y))

typedef struct ll_node_t ll_node_t;
struct ll_node_t {
    void *data;
    ll_node_t *next;
};

typedef struct dll_node_t dll_node_t;
struct dll_node_t {
    void *data;
    dll_node_t *next;
    dll_node_t *prev;
};

typedef struct dlinked_list_t dlinked_list_t;
struct dlinked_list_t {
    dll_node_t *head;
    unsigned int data_size;
    unsigned int size;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t {
    ll_node_t *head;
    unsigned int data_size;
    unsigned int size;
};

typedef struct stack_t stack_t;
struct stack_t {
    linked_list_t *list;
};

linked_list_t * ll_create(unsigned int data_size);
ll_node_t* ll_get_nth_node(linked_list_t* list, unsigned int n);
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);
ll_node_t * ll_remove_nth_node(linked_list_t* list, unsigned int n);

unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);

dlinked_list_t* dll_create(unsigned int data_size);
dll_node_t *dll_get_nth_node(dlinked_list_t* list, unsigned int n);
void dll_add_nth_node(dlinked_list_t* list, unsigned int n, const void* new_data);
dll_node_t* dll_remove_nth_node(dlinked_list_t* list, unsigned int n);

unsigned int dll_get_size(dlinked_list_t* list);
void dll_free(dlinked_list_t** pp_list);

stack_t * st_create(unsigned int data_size);
unsigned int st_get_size(stack_t *st);
unsigned int st_is_empty(stack_t *st);
void * st_peek(stack_t *st);

void st_pop(stack_t *st);
void st_push(stack_t *st, void *new_data);
void st_clear(stack_t *st);
void st_free(stack_t *st);

#endif

