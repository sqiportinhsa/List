#include <stdio.h>

#include "list.h"
#include "Libs\logging.h"

int main() {
    FILE *output = fopen("logs.txt", "w");
    SetLogStream(output);

    List list1 = {};
    list_ctr(&list1, 10);
    list_insert(&list1, 1, 0);
    dump_list(&list1);
    printf("%d ", list_pop(&list1, 1));
    printf("%d", list_verificator(&list1));
    dump_list(&list1);
    list_dtor(&list1);

    fclose(output);

    return 0;
}