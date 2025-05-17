/* URSESCU Sebastian - 315CA */
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

page_t *create_empty_page() {
    // create an empty page
    page_t *new_page = malloc(sizeof(page_t));
    DIE(!new_page, "page malloc failed");

    return new_page;
};

page_t *create_default_page() {
    // create a default page
    page_t *new_default_page = create_empty_page();

    new_default_page->id = 0;
    strcpy(new_default_page->url, DEFAULT_URL);

    new_default_page->description = malloc(sizeof(DEFAULT_URL));
    strcpy(new_default_page->description, DEFAULT_DESCRIPTION);

    return new_default_page;
}

page_t *create_page(int id, char url[50], char *desc) {
    // create a page from the given data
    page_t *new_page = create_empty_page();

    new_page->id = id;
    strcpy(new_page->url, url);

    new_page->description = malloc(strlen(desc) + 1);
    strcpy(new_page->description, desc);

    return new_page;
}

int get_last_id(browser_t *browser) {
    // function to get the last (current) id of the browser
    dlinked_list_t *list = browser->tablist.list;
    dll_node_t *last_node = list->head->prev;

    tab_t* tab = (tab_t *)(last_node->data);

    return tab->id;
}

tab_t *create_empty_tab(browser_t *browser) {
    // create an empty tab
    tab_t *new_tab = malloc(sizeof(tab_t));
    DIE(!new_tab, "tab malloc failed");

    // next id should be bigger than the last biggest id
    new_tab->id = get_last_id(browser) + 1;
    new_tab->current_page = create_default_page();
    new_tab->backward_stack = st_create(sizeof(page_t));
    new_tab->forward_stack = st_create(sizeof(page_t));

    return new_tab;
}

void add_tab(browser_t *browser, int last_id) {
    tab_t *new_tab = create_empty_tab(browser);
    new_tab->id = last_id;

    dll_add_nth_node(browser->tablist.list, browser->tablist.list->size, new_tab);
    free(new_tab);
    browser->current_tab = (tab_t *)(dll_get_nth_node(browser->tablist.list, browser->tablist.list->size - 1)->data);
}

int find_page_by_id(page_t *page_arr, int page_size, int page_id) {
    // function to return page_arr index from a given id
    for (int i = 0; i < page_size; i++) {
        if (page_arr[i].id == page_id) {
            return i;
        }
    }
    // couldn't find the page
    return -1;
}

void open_page(browser_t* browser, FILE *out, page_t *page_arr, int page_size, int page_id) {
    // open a new page
    int index = find_page_by_id(page_arr, page_size, page_id);

    if (index == -1) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }
    tab_t* tab = browser->current_tab;
    st_push(tab->backward_stack, tab->current_page);

    free(tab->current_page);

    // create new page
    page_t page_extract = page_arr[index];
    page_t *temp_page = create_page(page_extract.id,
                                    page_extract.url,
                                    page_extract.description);

    browser->current_tab->current_page = temp_page;

    // clear the forward stack
    page_t *b_peek = (page_t *)st_peek(tab->forward_stack);
    while (b_peek) {
        st_pop(tab->forward_stack);
        free_page(b_peek);
        b_peek = (page_t *)st_peek(tab->forward_stack);
    }

    // update the list
    unsigned int n = get_index_from_id(browser->tablist.list, browser->current_tab->id);

    dll_node_t *node = dll_remove_nth_node(browser->tablist.list, n);
    free(node);

    dll_add_nth_node(browser->tablist.list, n, browser->current_tab);
    free(browser->current_tab);
    browser->current_tab = (tab_t *)(dll_get_nth_node(browser->tablist.list, n)->data);
}

void open_tab(browser_t *browser, FILE *out, int id) {
    dll_node_t *current_node = browser->tablist.list->head;

    // we don't need to start searching with the sentinel
    current_node = current_node->next;

    tab_t *tab = (tab_t *)(current_node->data);

    while (tab->id != id && tab->id != -1) {
        current_node = current_node->next;
        tab = (tab_t *)(current_node->data);
    }
    if (tab->id == -1) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }
    browser->current_tab = tab;
}

tab_t *create_sentinel_tab() {
    tab_t *sentinel = malloc(sizeof(tab_t));
    DIE(!sentinel, "sentinel malloc failed");

    // we will use the id of -1 as the sentinel (santinela)
    sentinel->id = -1;
    sentinel->current_page = NULL;
    sentinel->backward_stack = NULL;
    sentinel->forward_stack = NULL;

    return sentinel;
}

unsigned int get_index_from_id(dlinked_list_t *list, int id) {
    unsigned int n = 0;

    dll_node_t *current_node = list->head;
    tab_t *tab = (tab_t *)(current_node->data);

    while (current_node && tab->id != id) {
        current_node = current_node->next;
        tab = (tab_t *)(current_node->data);
        n++;
        
        // we have cicled the list already, stop it
        if (current_node == list->head) {
            return 0;
        }
    }

    return n;
}

browser_t *create_browser(unsigned int data_size) {
    browser_t *new_browser = malloc(sizeof(browser_t));
    DIE(!new_browser, "browser malloc failed");

    dlinked_list_t *list = dll_create(data_size);
    new_browser->tablist.list = list;

    tab_t *sentinel = create_sentinel_tab();
    dll_add_nth_node(new_browser->tablist.list, new_browser->tablist.list->size, sentinel);
    free_tab(sentinel);

    tab_t *first_tab = create_empty_tab(new_browser);
    dll_add_nth_node(new_browser->tablist.list, new_browser->tablist.list->size, first_tab);
    free(first_tab);

    new_browser->current_tab = (tab_t *)(dll_get_nth_node(new_browser->tablist.list, 1)->data);

    return new_browser;
}

void free_page(page_t *page) {
    if (page) {
        if (page->id >= 0 && page->description) {
            free(page->description);
            page->description = NULL;
        }
        free(page);
        page = NULL;
    }
}

void free_tab(tab_t *tab) {
    if (tab) {
        if (tab->id != -1) {
            free_page(tab->current_page);

            page_t *b_peek = (page_t *)st_peek(tab->backward_stack);
            while (b_peek) {
                st_pop(tab->backward_stack);
                free_page(b_peek);
                b_peek = (page_t *)st_peek(tab->backward_stack);
            }
            st_free(tab->backward_stack);

            page_t *f_peek = (page_t *)st_peek(tab->forward_stack);
            while (f_peek) {
                st_pop(tab->forward_stack);
                free_page(f_peek);
                f_peek = (page_t *)st_peek(tab->forward_stack);
            }
            st_free(tab->forward_stack);
        }
        free(tab);
        tab = NULL;
    }
}

void free_browser(browser_t *browser) {
    unsigned int n = browser->tablist.list->size;

    while (n--) {
        dll_node_t *last_node = dll_remove_nth_node(browser->tablist.list, n);
        free_tab((tab_t *)(last_node->data));
        free(last_node);
    }

    free(browser->tablist.list);
    browser->tablist.list = NULL;
    free(browser);
    browser = NULL;
}

void close_current_tab(browser_t *browser, FILE *out) {
    tab_t *current_tab = browser->current_tab;

    if (current_tab->id <= 0) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }

    unsigned int n = get_index_from_id(browser->tablist.list, current_tab->id);
    dll_node_t *removed_node = dll_remove_nth_node(browser->tablist.list, n);

    free_tab(current_tab);
    browser->current_tab = (tab_t *)(removed_node->prev->data);
    free(removed_node);
}

void next_tab(browser_t *browser) {
    unsigned int n = get_index_from_id(browser->tablist.list, browser->current_tab->id);

    dll_node_t *node = dll_get_nth_node(browser->tablist.list, n);
    browser->current_tab = (tab_t *)(node->next->data);

    // if the new tab is the sentinel, skip it
    if (browser->current_tab->id == -1) {
        browser->current_tab = (tab_t *)(node->next->next->data);
    }
}

void prev_tab(browser_t *browser) {
    unsigned int n = get_index_from_id(browser->tablist.list, browser->current_tab->id);

    dll_node_t *node = dll_get_nth_node(browser->tablist.list, n);

    browser->current_tab = (tab_t *)(node->prev->data);

    // if the new tab is the sentinel, skip it
    if (browser->current_tab->id == -1) {
        browser->current_tab = (tab_t *)(node->prev->prev->data);
    }
}

void backward_page(browser_t *browser, FILE *out) {
    tab_t *tab = browser->current_tab;
    if (st_is_empty(tab->backward_stack)) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }

    page_t *peek_page = (page_t *)(st_peek(tab->backward_stack));

    st_pop(tab->backward_stack);
    st_push(tab->forward_stack, tab->current_page);
    free(tab->current_page);

    browser->current_tab->current_page = peek_page;
}

void forward_page(browser_t *browser, FILE *out) {
    tab_t *tab = browser->current_tab;
    if (st_is_empty(tab->forward_stack)) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }

    page_t *peek_page = (page_t *)(st_peek(tab->forward_stack));

    st_pop(tab->forward_stack);
    st_push(tab->backward_stack, tab->current_page);
    free(tab->current_page);

    browser->current_tab->current_page = peek_page;
}

void print_tabs(browser_t *browser, FILE *out) {
    unsigned int n = get_index_from_id(browser->tablist.list, browser->current_tab->id);

    dll_node_t *node = dll_get_nth_node(browser->tablist.list, n);
    tab_t *tab = (tab_t *)(node->data);
    int id = tab->id;

    fprintf(out, "%d", id);

    node = node->next;
    tab = (tab_t *)(node->data);

    while (tab->id != id) {
        if (tab->id == -1) {
            node = node->next;
            tab = (tab_t *)(node->data);
            continue;
        }
        fprintf(out, " %d", tab->id);
        node = node->next;
        tab = (tab_t *)(node->data);
    }
    fprintf(out, "\n");
}

void print_current_tab(browser_t *browser, FILE *out) {
    fprintf(out, "%s", browser->current_tab->current_page->description);
}

void print_recursive(FILE *out, ll_node_t *node) {
    if (node->next) {
        print_recursive(out, node->next);
    }
    page_t *page = (page_t *)(node->data);
    fprintf(out, "%s\n", page->url);
}

void print_history(browser_t *browser, FILE *out, int tab) {
    unsigned int n = get_index_from_id(browser->tablist.list, tab);

    if (n == 0) {
        fprintf(out, "%s\n", ERROR_MSG);
        return;
    }

    dll_node_t *node = dll_get_nth_node(browser->tablist.list, n);
    tab_t *current_tab = (tab_t *)(node->data);

    linked_list_t *f_list = current_tab->forward_stack->list;
    linked_list_t *b_list = current_tab->backward_stack->list;

    // print from forward stack
    ll_node_t *f_current_node = f_list->head;
    if (f_current_node) {
        print_recursive(out, f_current_node);
    }

    fprintf(out, "%s\n", current_tab->current_page->url);

    // print from backward stack
    ll_node_t *b_current_node = b_list->head;
    if (b_current_node) {
        page_t *page = (page_t *)(b_current_node->data);

        while (b_current_node) {
            page = (page_t *)(b_current_node->data);
            fprintf(out, "%s\n", page->url);

            b_current_node = b_current_node->next;
        }
    }
}

