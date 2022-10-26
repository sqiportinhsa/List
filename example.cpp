#include <stdio.h>

#include "list.h"
#include "Libs\logging.h"

int main() {
    FILE *output = fopen("logs.txt", "w");
    SetLogStream(output);

    List list1 = {};
    list_ctr(&list1, 10);
    for (int i = 1; i < 5; ++i) {
        list_insert(&list1, i, i-1);
    }
    dump_list(&list1);
    resize_list(&list1, 15, false);
    dump_list(&list1);
    list_dtor(&list1);

    fclose(output);

    return 0;
}