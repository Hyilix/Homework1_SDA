Doubly linked lists. Stacks - Tema 1
====================================

> URSESCU Sebastian - 315CA (Tema 1 SDA)

### Quick access

[General Overview](#general-overview)

[Pages](#pages)

[Tabs](#tabs)

[Browser](#browser)

[Data Structures](#data-structures)

[Commands](#commands)

[Implementation Details](#implementation-details)

[After Thoughts](#after-thoughts)

## General Overview

This project implements a browser simulator that emulates basic functions, including tab management, page navigation and browser history.
The system handles various commands such as creating/closing tabs, loading pages and managing history.

The browser will be able to open new tabs and open pages from a defined list, given at the start.

> The browser does not modify the initial list of pages

All the methods work by creating copies of data, so they can't be changed outside the data structures.
Thus, we need to be careful about the memory used to avoid duplicates.

To implement the browser, we first need to clarify the components of a browser:

## Main Components

### Pages

A page should store an id, url and description. The description is dynamicaly allocated and it always ends with endline.

```c
struct page_t {
  int id;
  char url[50];
  char *description
}
```

The default page is created when a new tab is created. It should look like this:

```
0
https://acs.pub.ro
Computer Science
```

A page is generally created using the **create_page** method, creating a page from a given id, url and description.
It is used to create a page from a template, or to make a copy from the list of defined pages.

To generate a default page, the **create_default_page** method is used.

### Tabs

Tabs store and id, the current page, and two stacks to store previously current pages. When created, the id assigned will be the biggest used id + 1 (even though the id is no longer used by any tabs).

```c
struct tab_t {
  int id;
  page_t *current_page;
  stack_t *backward_stack;
  stack_t *forward_stack;
}
```

A tab is generated using the **create_empty_tab** method. The id will be automatically generated. alongside a default page as the current page.

When a new page is opened from the page list, all the pages from the *forward_stack* are cleared, and the current page is pushed into the *backward_stack*.

Inside the current tab, pages can be cicled using **backward/forward_page** methods. The stacks will be updated, alongside the current page.

### Browser

The browser stores all the tabs (alongside all pages), and has a member to represent the current tab. All the functions affect the tab in some way.

```c
struct tablist_t {
  dlinked_list_t *list;
}
```

The doubly linked list used by the browser has a sentinel. While it's technically still a tab, it has no page or any stack, and has an id of -1.
It represents the head of the list and will be ignored.

The sentinel is created using the **create_sentinel_tab** method.

```c
struct browser_t {
  tab_t *current_tab;
  tablist_t tablist;
}
```

A browser is created using the **create_browser** method.

The current tab can be set using the **next/prev_tab** methods.
It just takes the next/previous node of current tab from the list.

Any tabs created will be updated in the *tablist* of the browser.

The browser is at the heart of all commands. There can be no more than one browser.

## Data Structures

### Doubly Linked Lists

Circular doubly linked lists are used for tab management.
Their circular natures provides ease of tab navigation.

Key operations:
* **dll_create** for list creation
* **dll_get_nth_node** for node accession
* **dll_add_nth_node** for adding to the list
* **dll_remove_nth_node** for removing

### Stack

Stacks are used to implement page history, in the form of *forward/backward_stack*.

It is implemented using a crude *linked list*.

Key operations:
* **st_create** for stack creation
* **st_peek** for page accession
* **st_push** for adding to the stack
* **st_pop** for removing from the stack
* **st_is_empty** to check if the stack is empty

## Commands

* NEW_TAB
  * Creates a new tab
  * The browser will select the new tab as current tab
* CLOSE
  * Closes the current tab
  * Will fail if the current tab has an id of 0 (first tab)
* OPEN \<id>
  * Opens a tab with the \<id> from the browser
  * The new tab will be the current tab
  * Will fail if no such tab exists
* NEXT
  * Switches the current tab to the next in the list
* PREV
  * Switches the current tab to the previous in the list
* PAGE \<id>
  * Opens a new page with the \<id> in the current tab from the list of available pages
* BACKWARD
  * Switches the current page to the previous in the list
  * Will fail if no pages exist in the backward list
* FORWARD
  * Switchest the current page to the next in the list
  * Will fail if no pages exist in the forward list
* PRINT
  * Prints all the open tabs
  * Prints the description of the current page of the current tab
* PRINT_HISTORY \<id>
  * Prints all opened pages from the tab with the \<id>
  * Print order: forward stack -> current page -> backward stack
    * The pages from the forward stack are recursively printed
    * The pages from the backward stack are iteratively printed
  * Will fail if no such tab exists

If any of these operations fail, **403 Forbidden** error will be showed.

## Implementation Details

Most methods used copy the element rather than saving it directly.
This protects the elements from outside modifications to the data.

### Browser Creation

For the **create_browser** method:
1. create the browser and the list
2. create a sentinel tab (id = -1) via the **create_sentinel_tab**, and push it to the list. This will be the list's head
3. create a new tab (id = 0), with a default page via the **create_empty_tab** method, and push it to the list
4. remove both the copies of sentinel and new tabs from memory

### Page History Navigation

For **backward/forward_page** methods:
1. get the top element from the stack
2. remove the top element from the stack
3. push the current page to the opposite stack
4. remove the copy of the current page from memory

### Tab Navigation

Using the **next/prev_tab** methods, the current tab will be assigned as the next/previous tab in the list.

The sentinel tab will be ignored in this operation.

### Tab Closing

Using the **close_current_tab** method, the current tab will be removed:
1. get the current tab from the list
2. remove the current tab of the browser
3. set the new current tab with the one recieved from the list

### Page Opening

To open a page from the list of pages, the **open_page** method is used:
1. push the current page into the backward stack
2. remove the copy of the current page
3. create the new page using the template of the one requested from the list of pages
4. clear the forward stack entirely (but don't free the memory of the stack)
5. assign the current page to the current tab
6. update the list with the changes to the current tab
7. remove any copies

### Memory Freeing

There are 3 levels of freeing:
1. page freeing
2. tab freeing
3. browser freeing

Each freeing method will be contained in the one below its respective level.

This ensures the complete clearing of memory.

## After Thoughts

After finishing this homework, I learned a lot about basic implementation and usage of lists and stacks.
This project is too basic and too crude to be called a browser, but there are some functions that are useful for a browser.
There are some places that are unpolished, some places that were left as they were if they worked. If it works, don't fix it. And the core data structures could be implemented smarter for this type of application.
Still, this presented an excellent opportunity to get my grips on data structures and to learn about some basic functions of a browser.
