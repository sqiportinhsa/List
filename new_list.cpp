#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "new_list.h"


static void set_free_cells(List_elem *data, size_t first, size_t last, size_t size);

static size_t calculate_new_size(const List *list, size_t new_size);

static int  check_position(const List *list, size_t position);
static int  cell_is_free(const List_elem *elem);
static int  verify_loop(const List *list);
static void dump_list_data(const List *list, FILE *output);

static int  generate_graph_code(const List *list);
static void generate_file_name(char *filename, const char *extension);


static int file_with_graphviz_code_counter = 0;
static const int max_file_with_graphviz_code_name_len = 15;
static const int max_generation_png_command_len = 30;
static const int max_png_file_name_len = 15;


int real_list_ctr(List *list, size_t list_size, const char *file, const char *func, int line) {
    if (list == nullptr) {
        dump_list(list, "can't create list: nullptr to it\n");
        return NULLPTR_TO_LIST;
    }

    list->data    = nullptr;
    list->cr_logs = nullptr;

    list->data = (List_elem*) calloc(list_size + 1, sizeof(List_elem));

    if (list->data == nullptr) {
        dump_list(list, "can't create list of size %zu: not enough memory\n", list_size);
        list_dtor(list);
        return NOT_ENOUGTH_MEM;
    }

    list->cr_logs = (Creation_logs*) calloc(1, sizeof(Creation_logs));

    if (list->data == nullptr) {
        dump_list(list, "can't create list of size %zu: not enough memory for logs\n", list_size);
        list_dtor(list);
        return NOT_ENOUGTH_MEM;
    }

    list->list_size  = list_size;
    list->busy_elems = 0;

    list->data[0].val  = data_poison;
    list->data[0].prev = 0;
    list->data[0].next = 0;

    set_free_cells(list->data, 1, list_size, list_size);

    list->free = 1;

    list->cr_logs->file_of_creation = file;
    list->cr_logs->func_of_creation = func;
    list->cr_logs->line_of_creation = line;

    return NO_LIST_ERRORS;
}

int list_dtor(List *list) {
    if (list == nullptr) {
        dump_list(list, "can't destruct list: pointer to it is nullptr\n");
        return NULLPTR_TO_LIST;
    }

    free(list->data);
    free(list->cr_logs);

    list->data    = nullptr;
    list->cr_logs = nullptr;

    list->busy_elems = 0;
    list->list_size  = 0;
    list->free       = 0;

    return NO_LIST_ERRORS;
}

size_t list_insert(List *list, Elem_t elem, size_t position) {
    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list,       errors, NULLPTR_TO_LIST,
                                   "Error: can't add element to non-existing list\n");
    CHECK_FOR_NULLPTR_WITH_MESSAGE(list->data, errors, NULLPTR_TO_DATA,
                                   "Error: can't add element because data ptr is nullptr\n");

    if (list->free == 0) {
        dump_list(list, "Error: can't add element to full list\n");
        return LIST_IS_FULL;
    }

    errors |= check_position(list, position);

    errors |= list_verificator(list);

    RETURN_IF(errors);

    size_t inserted = list->free;

    list->free = list->data[inserted].next;

    list->data[list->data[position].next].prev = inserted;

    list->data[inserted].next = list->data[position].next;
    list->data[inserted].prev = position;

    list->data[position].next = inserted;


    list->data[inserted].val  = elem;

    ++list->busy_elems;

    return inserted;
}

size_t list_insert_head(List *list, Elem_t elem) {
    int errors = NO_LIST_ERRORS;
    errors |= list_verificator(list);

    RETURN_IF(errors);

    return list_insert(list, elem, 0);
}

size_t list_insert_back(List *list, Elem_t elem) {
    int errors = NO_LIST_ERRORS;
    errors |= list_verificator(list);

    RETURN_IF(errors);

    return list_insert(list, elem, list->data[0].prev);
}

Elem_t list_pop(List *list, size_t position) {
    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list,       errors, NULLPTR_TO_LIST,
                                   "Error: can't pop element from non-existing list\n");
    CHECK_FOR_NULLPTR_WITH_MESSAGE(list->data, errors, NULLPTR_TO_DATA,
                                   "Error: can't pop element because data ptr is nullptr\n");

    if (position == 0) {
        dump_list(list, "Error: can't pop zero element of list\n");
        return 0;
    }

    errors |= check_position(list, position);

    if (errors != 0) {
        PrintToLogs("Incorrect pop");
        return 0;
    }

    size_t prev = list->data[position].prev;
    size_t next = list->data[position].next;

    Elem_t popped = list->data[position].val;

    list->data[prev].next = next;
    list->data[next].prev = prev;

    list->data[position].prev = prev_poison;
    list->data[position].next = list->free;
    list->data[position].val  = data_poison;

    list->free = position;

    --list->busy_elems;

    return popped;
}

Elem_t list_pop_head(List *list) {
    int errors = NO_LIST_ERRORS;
    errors |= list_verificator(list);

    RETURN_IF(errors);

    return list_pop(list, list->data[0].next);
}

Elem_t list_pop_back(List *list) {
    int errors = NO_LIST_ERRORS;
    errors |= list_verificator(list);

    RETURN_IF(errors);

    return list_pop(list, list->data[0].prev);
}


int list_verificator(const List *list) {
    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR(list, errors, NULLPTR_TO_LIST);

    RETURN_IF(errors & NULLPTR_TO_LIST);

    CHECK_FOR_NULLPTR(list->cr_logs, errors, NULLPTR_TO_LOGS);
    CHECK_FOR_NULLPTR(list->data,    errors, NULLPTR_TO_DATA);

    if (list->busy_elems > list->list_size) {
        errors |= BUSY_EXC_SIZE;
    }

    if (!(errors & BUSY_EXC_SIZE)) {
        errors |= verify_loop(list);
    }

    return errors;
}

int real_dump_list(const List *list, const char* file, const char* func, int line, const char *message, ...) {
    int errors = list_verificator(list);

    FILE *output = GetLogStream();

    fprintf(output, "List dump called in %s(%d), function %s\n", file, line, func);

    va_list ptr = {};
    va_start(ptr, message);
    vfprintf(output, message, ptr);
    va_end(ptr);
    
    if (errors & NULLPTR_TO_LIST) {
        fprintf(output, "Can't dump list from nullptr pointer\n");
        return errors;
    }

    fprintf(output, "List [%p] ", list);

    if (errors & NULLPTR_TO_LOGS) {
        fprintf(output, "without creation info (logs ptr in nullptr):\n");
    } else {
        fprintf(output, "created at %s(%d), function %s:\n", list->cr_logs->file_of_creation, 
                                                             list->cr_logs->line_of_creation, 
                                                             list->cr_logs->func_of_creation);
    }

    fprintf(output, "List info:\n");
    fprintf(output, "\tsize: %zu\n", list->list_size);
    fprintf(output, "\tbusy: %zu\n", list->busy_elems);
    fprintf(output, "\tfree: %zu\n", list->free);

    fflush(output);

    if (errors & NULLPTR_TO_DATA) {
        fprintf(output, "\tCan't print data: ptr to data is nullptr\n");
    } else {
        fprintf(output, "\tList data:\n");
        generate_graph_code(list);
    }

    fprintf(output, "\n");
    fflush(output);

    return errors;
}

int resize_list_without_sort(List *list, size_t new_size) {
    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_LIST, 
                                   "Error: can't resize non existing list\n");

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_DATA,
                                   "Error: can't resize list without data\n");

    new_size = calculate_new_size(list, new_size);

    if (new_size < list->free) {
        list->free = 0;
    }

    if (list->free == 0 && new_size > list->list_size) {
        list->free = list->list_size + 1;
    }

    list->data = (List_elem*) realloc(list->data, sizeof(List_elem) * (new_size + 1));

    set_free_cells(list->data, list->list_size + 1, new_size, new_size);

    list->list_size = new_size;

    return errors;
}

int resize_list_with_sort(List *list, size_t new_size) {
    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_LIST, 
                                   "Error: can't resize non existing list\n");

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_DATA,
                                   "Error: can't resize list without data\n");

    new_size = calculate_new_size(list, new_size);

    int elem_size = sizeof(List_elem);

    List_elem *new_data = (List_elem*) calloc(new_size + 1, elem_size);

    size_t pos_in_list = 0;

    new_data[0] = list->data[0];
    pos_in_list = list->data[0].next;
    new_data[0].next = 1;
    new_data[0].prev = list->busy_elems;

    for (size_t i = 1; i <= list->busy_elems; ++i) {
        new_data[i] = list->data[pos_in_list];
        pos_in_list = list->data[pos_in_list].next;
        new_data[i].next = (i + 1) % (new_size + 1);
        new_data[i].prev = (i - 1) % (new_size + 1);
    }

    set_free_cells(new_data, list->busy_elems + 1, new_size, new_size);

    free(list->data);
    list->data      = new_data;
    list->list_size = new_size;
    list->free      = (list->busy_elems + 1) % new_size;

    return errors;
}

size_t get_real_index_by_logical(List *list, size_t logical_index, int *errors) {
    int found_errors = NO_LIST_ERRORS;

    if (list == nullptr) {
        if (errors == nullptr) {
            return 0;
        }

        *errors |= NULLPTR_TO_LIST;
        return 0;
    }

    size_t real_index = 0;

    for (size_t i = 0; i < logical_index; ++i) {
        found_errors |= check_position(list, real_index);
        found_errors |= check_position(list, i);

        if (found_errors != 0) {
            if (errors == nullptr) {
                return 0;
            }

            *errors |= found_errors;
            return 0;
        }
        
        real_index = list->data[real_index].next;
    }

    return real_index;
}

static size_t calculate_new_size(const List *list, size_t new_size) {
    assert(list != nullptr);

    if (list->list_size > new_size) {
        size_t dif = list->list_size - new_size;

        for (size_t i = 0; i < dif; ++i) {
            if (!cell_is_free(&list->data[list->list_size - i])) {
                dif = i;
                break;
            }
        }

        new_size = list->list_size - dif;

    }

    return new_size;
}

static void set_free_cells(List_elem *data, size_t first, size_t last, size_t size) {
    assert(data != nullptr);

    for (size_t i = first; i <= last; ++i) {
        data[i].next = (i + 1) % (size + 1);
        data[i].val  = data_poison;
        data[i].prev = prev_poison;
    }
}

static void dump_list_data(const List *list, FILE *output) {
    assert(list != nullptr);
    assert(output != nullptr);

    fprintf(output, "\t\tindex: {");
    for (size_t i = 0; i <= list->list_size; ++i) {
        fprintf(output, "%8d ", (int) i);
    }
    fprintf(output, "}\n\t\tdata:  {");
    for (size_t i = 0; i <= list->list_size; ++i) {
        fprintf(output, "%8d ", list->data[i].val);
    }
    fprintf(output, "}\n\t\tprev:  {");
    for (size_t i = 0; i <= list->list_size; ++i) {
        fprintf(output, "%8zu ", list->data[i].prev);
    }
    fprintf(output, "}\n\t\tnext:  {");
    for (size_t i = 0; i <= list->list_size; ++i) {
        fprintf(output, "%8zu ", list->data[i].next);
    }
    fprintf(output, "}\n");
}

static int generate_graph_code(const List *list) {
    char code_filename[max_file_with_graphviz_code_name_len] = {};
    generate_file_name(code_filename, "dot");

    FILE *code_output = fopen(code_filename, "w");

    int errors = NO_LIST_ERRORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(code_output, errors, CANT_OPEN_FILE, 
                                   "Error: can't open file for graph generation\n");

    Print_code("digraph G{\n");
    Print_code("rankdir=LR;");
    Print_code("node [shape=record,style=\"filled\"];\n");
    Print_code("splines=ortho;\n");

    Print_code("info [label = \"List | size: %zu | busy: %zu |<f1>next free: %zu\"]", 
                                      list->list_size, list->busy_elems, list->free);
    if (list->free != 0) {
        Print_code("info:<f1>->node%zu [color=\"%s\",constraint=false];\n", 
                                             list->free, FREE_ARROW_COLOR);
    }
    Print_code("info->node0 [style=invis, weight = 100]\n");

    Print_reserved(list);
    Print_connection_arrow(list, (size_t) 0);
    Print_busy__next_arrow(list, (size_t) 0);
    Print_busy__prev_arrow(list, (size_t) 0);

    for (size_t i = 1; i < list->list_size; ++i) {
        if (cell_is_free(&list->data[i])) {
            Print_free(list, i);

            Print_connection_arrow(list, i);
            Print_free__next_arrow(list, i);
        } else {
            Print_busy(list, i);

            Print_connection_arrow(list, i);
            Print_busy__next_arrow(list, i);
            Print_busy__prev_arrow(list, i);
        }
    }

    if (cell_is_free(&list->data[list->list_size])) {
        Print_free            (list, list->list_size);
        Print_free__next_arrow(list, list->list_size);
    } else {
        Print_busy            (list, list->list_size);
        Print_busy__next_arrow(list, list->list_size);
        Print_busy__prev_arrow(list, list->list_size);
    }
    
    Print_code("}");

    fclose(code_output);

    char command[max_generation_png_command_len] = {};
    char png_file_name[max_png_file_name_len] = {};
    generate_file_name(png_file_name, "png");
    sprintf(command, " c:\\GitHub\\List\\Libs\\Graphviz\\bin\\dot.exe %s -o %s -T png", 
                                                         code_filename, png_file_name);
    
    if (system(command) != 0) {
        PrintToLogs("Error: can't generate picture\n");
        return CANT_GENER_PIC;
    }

    #ifdef LOGS_TO_HTML
    PrintToLogs("\n<img src=\"%s\">\n", png_file_name);
    #else
    PrintToLogs("Picture is generated. You can find it by name %s.\n", png_file_name);
    #endif

    return NO_LIST_ERRORS;
}

static void generate_file_name(char *filename, const char *extension)  {
    sprintf(filename, "graph_%d.%s", file_with_graphviz_code_counter, extension);
    ++file_with_graphviz_code_counter;
}

static int check_position(const List *list, size_t position) {
    assert(list != nullptr);
    
    if (position > list->list_size) {
        return POS_DONT_EXIST;
    }
    return NO_LIST_ERRORS;
}

static int cell_is_free(const List_elem *elem) {
    return (elem->val == data_poison && elem->prev == prev_poison);
}

static int verify_loop(const List *list) {
    int errors = 0;

    CHECK_FOR_NULLPTR(list,       errors, NULLPTR_TO_LIST);
    CHECK_FOR_NULLPTR(list->data, errors, NULLPTR_TO_DATA);

    RETURN_IF(errors & NULLPTR_TO_LIST);
    RETURN_IF(errors & NULLPTR_TO_DATA);

    if (list->busy_elems == 0) {
        if (list->data[0].next == 0 && list->data[0].prev == 0) {
            return NO_LIST_ERRORS;
        }
        return BROKEN_LOOP;
    }

    size_t next = list->data[0].next;

    for (size_t i = 0; i < list->busy_elems - 1; ++i) {
        next = list->data[next].next;
        if (next == 0 || check_position(list, next)) {
            return (BROKEN_LOOP);
        }
    }

    next = list->data[next].next;

    if (next != 0) {
        return BROKEN_LOOP;
    }

    return NO_LIST_ERRORS;
}
