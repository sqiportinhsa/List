#include <stdio.h>

#include "new_list.h"
#include "Libs/logging.h"

int main() {
    FILE *output = fopen("logs.html", "w");
    SetLogStream(output);

    PrintToLogs("<pre>\n");

    List list1 = {};
    list_ctr(&list1, 10);

    PrintToLogs("Freshy created list:\n");
    dump_list(&list1);

    for (int i = 1; i < 5; ++i) {
        list_insert(&list1, i, i-1);
    }

    PrintToLogs("List after adding 5 elements consequentially\n");
    dump_list(&list1);

    for (int i = 6; i < 10; ++i) {
        list_insert(&list1, i, i-3);
        PrintToLogs("List after adding element %d to position %d\n", i, i-3);
        dump_list(&list1);
    }

    resize_list_without_sort(&list1, 5);

    PrintToLogs("List after resizing without sorting to size less than amount of busy elements\n");
    dump_list(&list1);

    resize_list_without_sort(&list1, 15);

    PrintToLogs("List after resizing without sorting to size more than amount of busy elements\n");
    dump_list(&list1);

    resize_list_with_sort(&list1, 1);

    PrintToLogs("List after resizing with sorting to size less than amount of busy elements\n");
    dump_list(&list1);

    resize_list_with_sort(&list1, 15);

    PrintToLogs("List after resizing with sorting to size more than amount of busy elements\n");
    dump_list(&list1);

    PrintToLogs("Poped element from position %d: %d\n List after popping:\n", 5, list_pop(&list1, 5));
    dump_list(&list1);

    PrintToLogs("Poped first element: %d\n List after popping:\n", 5, list_pop_head(&list1));
    dump_list(&list1);

    PrintToLogs("Poped last element: %d\n List after popping:\n", 5, list_pop_back(&list1));
    dump_list(&list1);

    list_insert_head(&list1, 14);

    PrintToLogs("List after inserting element to the first position:\n");
    dump_list(&list1);

    list_insert_back(&list1, 17);

    PrintToLogs("List after inserting element to the last position:\n");
    dump_list(&list1);
    
    list_dtor(&list1);

    PrintToLogs("List after destruction:\n");
    dump_list(&list1);

    fclose(output);

    return 0;
}