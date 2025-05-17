/* URSESCU Sebastian - 315CA */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "data_structures.h"
#include "utils.h"

#define FILE_INPUT "tema1.in"
#define FILE_OUTPUT "tema1.out"
#define MAX_DESC_SIZE 100

typedef enum commands_t commands_t;
enum commands_t {
    NEW_TAB,
    CLOSE,
    OPEN,
    NEXT,
    PREV,
    PAGE,
    BACKWARD,
    FORWARD,
    PRINT,
    PRINT_HISTORY,

    NO_COMMANDS
};

typedef struct {
    commands_t command;
    int optional_arg;
} command_t;

commands_t str_to_command(char *str) {
    if (strcmp(str, "NEW_TAB") == 0)
        return NEW_TAB;
    if (strcmp(str, "CLOSE") == 0)
        return CLOSE;
    if (strcmp(str, "OPEN") == 0)
        return OPEN;
    if (strcmp(str, "NEXT") == 0)
        return NEXT;
    if (strcmp(str, "PREV") == 0)
        return PREV;
    if (strcmp(str, "PAGE") == 0)
        return PAGE;
    if (strcmp(str, "BACKWARD") == 0)
        return BACKWARD;
    if (strcmp(str, "FORWARD") == 0)
        return FORWARD;
    if (strcmp(str, "PRINT") == 0)
        return PRINT;
    if (strcmp(str, "PRINT_HISTORY") == 0)
        return PRINT_HISTORY;
    return NO_COMMANDS;
}

command_t dissect_command(char *str) {
    command_t new_command;
    new_command.optional_arg = -1;
    char *token = strtok(str, " ");

    if (token) {
        new_command.command = str_to_command(token);
        token = strtok(NULL, " ");
    }
    if (token) {
        new_command.optional_arg = atoi(token);
        if (atoi(token) == 0 && strcmp(token, "0") != 0) {
            new_command.optional_arg = -1;
        }
    }
    return new_command;
}

void clear_endline(char *str) {
    unsigned int i = 0;
    while (str[i] != '\0' && str[i] != '\n') {
        i++;
    }
    str[i] = '\0';
}

int main(void) {
    int pages, instructions;

    FILE *in = fopen(FILE_INPUT, "r");
    DIE(!in, "input file open failure");

    fscanf(in, "%d", &pages);

    // allocate memory to store all the pages
    page_t *page_array = malloc(sizeof(page_t) * pages);

    // populate the array of pages
    // NOTE: the array just stores the templates for the pages
    // the pages used by the tabs will be dinamically alloced. 
    for (int i = 0; i < pages; i++) {
        int id;
        char url[50];
        char *desc = NULL;

        fscanf(in, "%d\n", &id);
        fgets(url, 50, in);
        clear_endline(url);

        desc = malloc(MAX_DESC_SIZE);
        DIE(!desc, "description malloc failed");

        desc[0] = '\0';

        if (fgets(desc, MAX_DESC_SIZE, in)) {
            size_t len = strlen(desc);

            page_array[i].id = id;
            strcpy(page_array[i].url, url);

            page_array[i].description = malloc(len + 1);
            DIE(!page_array[i].description, "description malloc failed");

            strcpy(page_array[i].description, desc);
            free(desc);
        }
        else {
            printf("FAILED TO READ LINE\n");
            return 0;
        }

    }

    fscanf(in, "%d\n", &instructions);

    FILE *out = fopen(FILE_OUTPUT, "w");
    DIE(!out, "output file open failure");

    browser_t *browser = create_browser(sizeof(tab_t));

    command_t new_command;
    int last_id = 0;

    while (instructions--) {
        char com_str[50];
        fgets(com_str, 50, in);
        clear_endline(com_str);

        new_command = dissect_command(com_str);
        int opt_arg = new_command.optional_arg;

        switch (new_command.command) {
        case NEW_TAB:
            add_tab(browser, ++last_id);
            break;
        case CLOSE:
            close_current_tab(browser, out);
            break;
        case OPEN:
            open_tab(browser, out, opt_arg);
            break;
        case NEXT:
            next_tab(browser);
            break;
        case PREV:
            prev_tab(browser);
            break;
        case PAGE:
            open_page(browser, out, page_array, pages, opt_arg);
            break;
        case BACKWARD:
            backward_page(browser, out);
            break;
        case FORWARD:
            forward_page(browser, out);
            break;
        case PRINT:
            print_tabs(browser, out);
            print_current_tab(browser, out);
            break;
        case PRINT_HISTORY:
            print_history(browser, out, opt_arg);
            break;
        default:
            fprintf(out, "UNKNOWN COMMAND\n");
        }
    }

    for (int i = 0; i < pages; i++) {
        free(page_array[i].description);
    }
    free(page_array);
    free_browser(browser);

    fclose(in);
    fclose(out);
    return 0;
}

