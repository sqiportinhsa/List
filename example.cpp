#include <stdio.h>

#include "new_list.h"
#include "Libs/logging.h"

int main() {
    FILE *output = CreateLogFile("logs.html");

    List list1 = {};
    list_ctr(&list1, 10);

    dump_list(&list1, "Freshy created list:\n");

    for (int i = 1; i < 5; ++i) {
        list_insert(&list1, i, i-1);
    }

    dump_list(&list1, "List after adding 4 elements consequentially\n");

    for (int i = 6; i < 10; ++i) {
        list_insert(&list1, i, i-3);
        dump_list(&list1, "List after adding element %d to position %d\n", i, i-3);
    }

    resize_list_without_sort(&list1, 5);

    dump_list(&list1, "List after resizing without sorting to size less than amount of busy elements\n");

    resize_list_without_sort(&list1, 15);

    dump_list(&list1, "List after resizing without sorting to size more than amount of busy elements\n");

    resize_list_with_sort(&list1, 1);

    dump_list(&list1, "List after resizing with sorting to size less than amount of busy elements\n");

    resize_list_with_sort(&list1, 15);

    dump_list(&list1, "List after resizing with sorting to size more than amount of busy elements\n");

    dump_list(&list1, "Poped element from position %d: %d. List after popping:\n", 5, list_delete(&list1, 5));

    dump_list(&list1, "Poped first element: %d. List after popping:\n", list_pop_head(&list1));

    dump_list(&list1, "Poped last element: %d. List after popping:\n", list_pop_back(&list1));

    list_insert_head(&list1, 14);

    dump_list(&list1, "List after inserting element to the first position:\n");

    list_insert_back(&list1, 17);

    dump_list(&list1, "List after inserting element to the last position:\n");
    
    list_dtor(&list1);

    dump_list(&list1, "List after destruction:\n");

    fclose(output);

    return 0;
}