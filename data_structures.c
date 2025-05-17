/* URSESCU Sebastian - 315CA */
#include "data_structures.h"

#include <stdlib.h>
#include <string.h>

// Various data structure implementations

linked_list_t * ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));
    DIE(ll == NULL, "ll_create malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

ll_node_t* ll_get_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list) {
        return NULL;
    }

    unsigned int len = list->size - 1;
    unsigned int i;
    ll_node_t* node = list->head;

    n = MIN(n, len);

    for (i = 0; i < n; ++i) {
        node = node->next;
    }

    return node;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

ll_node_t * ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return 0;
    }

    return list->size;
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t* curr_node;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        curr_node = ll_remove_nth_node(*pp_list, 0);
        free(curr_node->data);
        curr_node->data = NULL;
        free(curr_node);
        curr_node = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

dlinked_list_t* dll_create(unsigned int data_size)
{
    // create list
    dlinked_list_t *d_list = (dlinked_list_t *)malloc(sizeof(dlinked_list_t));
    d_list->data_size = data_size;
    d_list->size = 0;

    d_list->head = NULL;

    return d_list;
}

dll_node_t *dll_get_nth_node(dlinked_list_t* list, unsigned int n) {
    if (list == NULL || list->size == 0) {
        return NULL;
    }
    
    // Handle negative indices or indices >= size by wrapping around
    if (list->size > 0) {
        n = ((n % list->size) + list->size) % list->size;
    }
    
    dll_node_t* current = list->head;
    unsigned int i;
    
    // Optimize traversal by choosing the shorter path
    if (n <= list->size / 2) {
        // printf("GO FORWARD\n");
        // Go forward
        for (i = 0; i < n; i++) {
            current = current->next;
        }
    } else {
        // Go backward
        for (i = 0; i < list->size - n; i++) {
            current = current->prev;
        }
    }
    
    return current;

}
void dll_add_nth_node(dlinked_list_t* list, unsigned int n, const void* new_data)
{
    // initialize new node
    dll_node_t *temp_node = (dll_node_t*)malloc(sizeof(dll_node_t));
    temp_node->data = (void *)malloc(list->data_size);

    memcpy(temp_node->data, new_data, list->data_size);
    temp_node->next = temp_node;
    temp_node->prev = temp_node;

    dll_node_t *current_node = list->head;

    if (current_node == NULL) {
        list->head = temp_node;
        list->size++;
        return;
    }

    if (n >= list->size) {
        current_node = current_node->prev;
        current_node->next = temp_node;
        temp_node->prev = current_node;
        temp_node->next = list->head;
        list->head->prev = temp_node;
    }
    else {
        for (unsigned int i = 0; i < n; i++) {
            current_node = current_node->next;
        }
        current_node = current_node->prev;
        temp_node->prev = current_node;
        temp_node->next = current_node->next;
        current_node->next->prev = temp_node;
        current_node->next = temp_node;
    }
    if (n == 0) {
        list->head = temp_node;
    }
    list->size++;
}

dll_node_t* dll_remove_nth_node(dlinked_list_t* list, unsigned int n)
{
    dll_node_t *current_node = list->head;

    if (current_node == NULL) {
        return NULL;
    }

    // remove first
    if (n == 0) {
        current_node->prev->next = current_node->next;
        current_node->next->prev = current_node->prev;
        list->head = current_node->next;
    }

    // determine if it's last node
    else if (n >= list->size - 1) {
        current_node = current_node->prev;
        current_node->prev->next = current_node->next;
        current_node->next->prev = current_node->prev;
    }

    // it's somewhere in between
    else {
        while (n > 0 && n-- > 0) {
            current_node = current_node->next;
        }
        current_node->prev->next = current_node->next;
        current_node->next->prev = current_node->prev;
    }
    list->size--;
    return current_node;
}

stack_t * st_create(unsigned int data_size)
{
    stack_t *new_stack = (stack_t *)malloc(sizeof(stack_t));

    linked_list_t *list = ll_create(data_size);
    list->head = NULL;
    list->size = 0;
    list->data_size = data_size;

    new_stack->list = list;
    return new_stack;
}

unsigned int dll_get_size(dlinked_list_t* list)
{
    return list->size;
}

void dll_free(dlinked_list_t** pp_list)
{
    dll_node_t *current_node = (*pp_list)->head;
    if (current_node) {
        // break the chain for easier iteration
        current_node->prev->next = NULL;
        current_node->prev = NULL;

        while (current_node->next) {
            current_node = current_node->next;

            // free the node and it's contents
            current_node->prev->prev = NULL;
            current_node->prev->next = NULL;
            free(current_node->prev->data);
            free(current_node->prev);
        }

        // free the last node
        current_node->prev = NULL;
        current_node->next = NULL;
        free(current_node->data);
        free(current_node);
    }
    (*pp_list)->head = NULL;
    free(*pp_list);
}

unsigned int st_get_size(stack_t *st)
{
    if (!st)
        return 0;
    
    int counter = 0;
    ll_node_t *current_node = st->list->head;

    while (current_node) {
        counter++;
        current_node = current_node->next;
    }
    return counter;
}

unsigned int st_is_empty(stack_t *st)
{
    if (st && st->list && st->list->head) {
        return 0;
    }

    return 1;
}

void * st_peek(stack_t *st)
{
    if (st && st->list->head)
        return st->list->head->data;
    return NULL;
}

void st_pop(stack_t *st)
{
    if (st == NULL) 
        return;

    ll_node_t *head = st->list->head;

    if (head) {
        if (head->next) {
            st->list->head = head->next;
        }
        else {
            st->list->head = NULL;
        }
        head->next = NULL;
    }
    free(head);
}

void st_push(stack_t *st, void *new_data)
{
    if (st == NULL) 
        return;

    ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));

    new_node->data = (void *)malloc(st->list->data_size);
    new_node->next = NULL;

    memcpy(new_node->data, new_data, st->list->data_size);

    ll_node_t *head = st->list->head;

    if (head) {
        new_node->next = head;
        st->list->head = new_node;
    }
    else {
        st->list->head = new_node;
    }
}

void st_clear(stack_t *st)
{
    while (st->list->head) {
        st_pop(st);
    }
}

void st_free(stack_t *st)
{
    if (st) {
        free(st->list);
        st->list = NULL;
        free(st);
        st = NULL;
    }
}

