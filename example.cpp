#include <stdio.h>

#include "list.h"

int main() {
    List list1 = {};
    list_ctr(&list1, 10);
    list_insert(&list1, 1, 0);
    printf("%d", list_pop(&list1, 1));
    list_dtor(&list1);
    return 0;
}