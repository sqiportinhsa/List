#include <stdio.h>
#include <stdlib.h>

#include "list.h"

int real_list_ctr(List *list, size_t list_size, const char *file, const char *func, int line) {
    if (list == nullptr) {
        fprintf(stderr, "can't create list: nullptr to it\n");
        return NULLPTR_TO_LIST;
    }

    list->data    = nullptr;
    list->cr_logs = nullptr;

    list->data = (List_elem*) calloc(list_size + 1, sizeof(List_elem));

    if (list->data == nullptr) {
        fprintf(stderr, "can't create list of size %zu: not enough memory\n", list_size);
        list_dtor(list);
        return NOT_ENOUGTH_MEM;
    }

    list->cr_logs = (Creation_logs*) calloc(1, sizeof(Creation_logs));

    if (list->data == nullptr) {
        fprintf(stderr, "can't create list of size %zu: not enough memory for logs\n", list_size);
        list_dtor(list);
        return NOT_ENOUGTH_MEM;
    }

    list->list_size  = list_size;
    list->busy_elems = 0;

    list->data[0].val  = data_poison;
    list->data[0].prev = 0;
    list->data[0].next = 0;

    for (size_t i = 1; i <= list_size; ++i) {
        list->data[i].next = (i + 1) % list_size;
        list->data[i].val  = data_poison;
        list->data[i].prev = prev_poison;
    }

    list->free = 1;

    list->cr_logs->file_of_creation = file;
    list->cr_logs->func_of_creation = func;
    list->cr_logs->line_of_creation = line;

    return NO_LIST_ERORS;
}

int list_dtor(List *list) {
    if (list == nullptr) {
        fprintf(stderr, "can't destruct list: pointer to it is nullptr\n");
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

    if (list == nullptr) {
        fprintf(stderr, "Error: can't add element to non-existing list\n");
        return NULLPTR_TO_LIST;
    }

    if (list->free == 0) {
        fprintf(stderr, "Error: can't add element to full list\n");
        return LIST_IS_FULL;
    }

    errors |= check_position(list, position);

    RETURN_IF(errors);

    size_t next = list->free;

    list->data[position].next = next;

    list->free = -(list->data[next].next);

    list->data[next].next = 0;
    list->data[next].prev = position;
    list->data[next].val  = elem;

    ++list->busy_elems;

    return next;
}

Elem_t list_pop(List *list, size_t position) {
    if (list == nullptr) {
        fprintf(stderr, "Error: can't pop element to non-existing list\n");
        return NULLPTR_TO_LIST;
    }

    if (position == 0) {
        fprintf(stderr, "Error: can't pop zero element of list\n");
    }

    size_t prev = list->data[position].prev;
    size_t next = list->data[position].next;

    Elem_t popped = list->data[position].val;

    list->data[prev].next = next;
    list->data[next].prev = prev;

    list->data[position].prev = -1;
    list->data[position].next = list->free;
    list->data[position].val  = data_poison;

    list->free = position;

    return popped;
}

static int check_position(const List *list, size_t position) {
    if (position > list->list_size) {
        return POS_DONT_EXIST;
    }
    return NO_LIST_ERORS;
}

static int cell_is_free(const List_elem* elem) {
    return (elem->val == data_poison && elem->prev == prev_poison);
}

static int verify_loop(const List *list) {
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

int list_verificator(const List *list) {
    int errors = NO_LIST_ERORS;

    CHECK_FOR_NULLPTR(list, errors, NULLPTR_TO_LIST);

    RETURN_IF(errors & NULLPTR_TO_LIST);

    CHECK_FOR_NULLPTR(list->cr_logs, errors, NULLPTR_TO_LOGS);
    CHECK_FOR_NULLPTR(list->data,    errors, NULLPTR_TO_DATA);

    errors |= verify_loop(list);

    return errors;
}
