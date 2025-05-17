/* URSESCU Sebastian - 315CA */
#ifndef UTILS_H
#define UTILS_H

#include "data_structures.h"

// default strings
#define DEFAULT_URL "https://acs.pub.ro/"
#define DEFAULT_DESCRIPTION "Computer Science\n"

#define ERROR_MSG "403 Forbidden"

typedef struct page_t page_t;
struct page_t {
    int id;
    char url[50];
    char *description;
};

typedef struct tab_t tab_t;
struct tab_t {
    int id;
    page_t *current_page;
    stack_t *backward_stack;
    stack_t *forward_stack;
};

typedef struct tablist_t tablist_t;
struct tablist_t {
    dlinked_list_t *list;
};

typedef struct browser_t browser_t;
struct browser_t {
    tab_t *current_tab;
    tablist_t tablist; // this could be a simple 'dlinked_list_t *list'
};

page_t *create_empty_page();
page_t *create_default_page();
page_t *create_page(int id, char url[50], char *desc);

int get_last_id(browser_t *browser);
tab_t *create_empty_tab(browser_t *browser);

void add_tab(browser_t *browser, int last_id);
void open_page(browser_t *browser, FILE *out, page_t *page_arr, int page_size, int page_id);
void open_tab(browser_t *browser, FILE *out, int id);

tab_t *create_sentinel_tab();
browser_t *create_browser(unsigned int data_size);

unsigned int get_index_from_id(dlinked_list_t *list, int id);

void free_page(page_t *page);
void free_tab(tab_t *tab);
void free_browser(browser_t *browser);

void close_current_tab(browser_t *browser, FILE *out);

void next_tab(browser_t *browser);
void prev_tab(browser_t *browser);

void backward_page(browser_t *browser, FILE *out);
void forward_page(browser_t *browser, FILE *out);

void print_tabs(browser_t *browser, FILE *out);
void print_current_tab(browser_t *browser, FILE *out);

void print_recursive(FILE *out, ll_node_t *node);
void print_history(browser_t *browser, FILE *out, int tab);

#endif

