#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>

#include "list.h"


static void set_free_cells(List_elem *data, size_t first, size_t last, size_t size);

static size_t calculate_new_size(List *list, size_t new_size);

static int check_position(const List *list, size_t position);
static int cell_is_free(const List_elem *elem);
static int verify_loop(const List *list);
static void dump_list_data(const List *list, FILE *output);

int real_list_ctr(List *list, size_t list_size, const char *file, const char *func, int line) {
    if (list == nullptr) {
        fprintf(stderr, "can't create list: nullptr to it\n");
        dump_list(list);
        return NULLPTR_TO_LIST;
    }

    list->data    = nullptr;
    list->cr_logs = nullptr;

    list->data = (List_elem*) calloc(list_size + 1, sizeof(List_elem));

    if (list->data == nullptr) {
        fprintf(stderr, "can't create list of size %zu: not enough memory\n", list_size);
        dump_list(list);
        list_dtor(list);
        return NOT_ENOUGTH_MEM;
    }

    list->cr_logs = (Creation_logs*) calloc(1, sizeof(Creation_logs));

    if (list->data == nullptr) {
        fprintf(stderr, "can't create list of size %zu: not enough memory for logs\n", list_size);
        dump_list(list);
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

    return NO_LIST_ERORS;
}

int list_dtor(List *list) {
    if (list == nullptr) {
        fprintf(stderr, "can't destruct list: pointer to it is nullptr\n");
        dump_list(list);
        return NULLPTR_TO_LIST;
    }

    free(list->data);
    free(list->cr_logs);

    list->data    = nullptr;
    list->cr_logs = nullptr;

    list->busy_elems = 0;
    list->list_size  = 0;
    list->free       = 0;

    return NO_LIST_ERORS;
}

size_t list_insert(List *list, Elem_t elem, size_t position) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list,       errors, NULLPTR_TO_LIST,
                                   "Error: can't add element to non-existing list\n");
    CHECK_FOR_NULLPTR_WITH_MESSAGE(list->data, errors, NULLPTR_TO_DATA,
                                   "Error: can't add element because data ptr is nullptr\n");

    if (list->free == 0) {
        log("Error: can't add element to full list\n");
        dump_list(list);
        return LIST_IS_FULL;
    }

    errors |= check_position(list, position);

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

Elem_t list_pop(List *list, size_t position) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list,       errors, NULLPTR_TO_LIST,
                                   "Error: can't pop element from non-existing list\n");
    CHECK_FOR_NULLPTR_WITH_MESSAGE(list->data, errors, NULLPTR_TO_DATA,
                                   "Error: can't pop element because data ptr is nullptr\n");

    if (position == 0) {
        fprintf(stderr, "Error: can't pop zero element of list\n");
        dump_list(list);
        return POP_ZERO_ELEM;
    }

    errors |= check_position(list, position);

    RETURN_IF(errors & POS_DONT_EXIST);

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

int list_verificator(const List *list) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR(list, errors, NULLPTR_TO_LIST);

    RETURN_IF(errors & NULLPTR_TO_LIST);

    CHECK_FOR_NULLPTR(list->cr_logs, errors, NULLPTR_TO_LOGS);
    CHECK_FOR_NULLPTR(list->data,    errors, NULLPTR_TO_DATA);

    

    errors |= verify_loop(list);

    return errors;
}

int real_dump_list(const List *list, const char* file, const char* func, int line) {
    int errors = list_verificator(list);

    FILE *output = GetLogStream();

    fprintf(output, "List dump called in %s(%d), function %s\n", file, line, func);
    
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
        dump_list_data(list, output);
    }

    fprintf(output, "\n");
    fflush(output);

    return errors;
}

int resize_list_without_sort(List *list, size_t new_size) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_LIST, 
                                   "Error: can't resize non existing list\n");

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_DATA,
                                   "Error: can't resize list without data\n");

    new_size = calculate_new_size(list, new_size);

    list->data = (List_elem*) realloc(list->data, sizeof(List_elem) * (new_size + 1));

    set_free_cells(list->data, list->list_size, new_size, new_size);

    list->list_size = new_size;

    return errors;
}

int resize_list_with_sort(List *list, size_t new_size) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_LIST, 
                                   "Error: can't resize non existing list\n");

    CHECK_FOR_NULLPTR_WITH_MESSAGE(list, errors, NULLPTR_TO_DATA,
                                   "Error: can't resize list without data\n");

    new_size = calculate_new_size(list, new_size);

    int elem_size = sizeof(List_elem);

    List_elem *new_data = (List_elem*) calloc(new_size + 1, elem_size);

    size_t pos_in_list = 0;

    for (size_t i = 0; i <= list->busy_elems; ++i) {
        new_data[i] = list->data[pos_in_list];
        pos_in_list = list->data[pos_in_list].next;
    }

    set_free_cells(new_data, list->busy_elems + 1, new_size, new_size);

    free(list->data);
    list->data      = new_data;
    list->list_size = new_size;

    return errors;
}

static size_t calculate_new_size(List *list, size_t new_size) {
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

static int check_position(const List *list, size_t position) {
    assert(list != nullptr);
    
    if (position > list->list_size) {
        return POS_DONT_EXIST;
    }
    return NO_LIST_ERORS;
}

static int cell_is_free(const List_elem *elem) {
    return (elem->val == data_poison && elem->prev == prev_poison);
}

static int verify_loop(const List *list) {
    int errors = 0;

    CHECK_FOR_NULLPTR(list, errors, NULLPTR_TO_LIST);

    RETURN_IF(errors & NULLPTR_TO_LIST);

    if (list->busy_elems == 0) {
        if (list->data[0].next == 0 && list->data[0].prev == 0) {
            return NO_LIST_ERORS;
        }
        return BROKEN_LOOP;
    }

    size_t next = list->data[0].next;

    for (size_t i = 0; i < list->busy_elems - 1; ++i) {
        next = list->data[next].next;
        if (next == 0) {
            return (BROKEN_LOOP);
        }
    }

    next = list->data[next].next;

    if (next != 0) {
        return BROKEN_LOOP;
    }

    return NO_LIST_ERORS;
}
