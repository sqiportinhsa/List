#ifndef LIST
#define LIST

#include "Libs/logging.h"

typedef int Elem_t;

const Elem_t data_poison = 0x7011A;
const Elem_t prev_poison = 0x011A7;

typedef struct {
    size_t prev = 0;
    size_t next = 0;
    Elem_t  val = 0;
} List_elem;

typedef struct {
    Creation_logs* cr_logs = nullptr;
    List_elem*        data = nullptr;
    size_t      busy_elems = 0;
    size_t       list_size = 0;
    size_t            free = 0;
} List;

typedef enum {
    NO_LIST_ERORS   = 0,
    NULLPTR_TO_LIST = 1 << 0,
    NULLPTR_TO_DATA = 1 << 1,
    NULLPTR_TO_LOGS = 1 << 2,
    NOT_ENOUGTH_MEM = 1 << 3,
    POP_ZERO_ELEM   = 1 << 4,
    LIST_IS_FULL    = 1 << 5,
    POS_DONT_EXIST  = 1 << 6,
    BROKEN_LOOP     = 1 << 7,
    NON_POISON_FREE = 1 << 8,
    CANNOT_RESIZE   = 1 << 9,
} List_errors;

#define RETURN_IF(error) if (error)      \
                            return errors;

#define CHECK_FOR_NULLPTR(ptr, errors, error_name) if(ptr == nullptr)       \
                                                        errors |= error_name;

#define CHECK_FOR_NULLPTR_WITH_MESSAGE(ptr, errors, error_name, message) \
        if (ptr == nullptr) {                                            \
            errors |= error_name;                                        \
            fprintf(stderr, message);                                    \
            return errors;                                               \
        }

#define list_ctr(list, list_size) real_list_ctr(list, list_size, __FILE__, __PRETTY_FUNCTION__, \
                                                                                        __LINE__)

#define dump_list(list) real_dump_list(list, __FILE__, __PRETTY_FUNCTION__, __LINE__)

int real_list_ctr(List *list, size_t list_size, const char *file, const char *func, int line);
int list_dtor(List *list);
size_t list_insert(List *list, Elem_t elem, size_t position);
int list_pop(List *list, size_t position);
int list_verificator(const List *list);
int real_dump_list(const List *list, const char* file, const char* func, int line);
int resize_list(List *list, size_t new_size, bool sort);
static int check_position(const List *list, size_t position);
static int cell_is_free(const List_elem *elem);
static int verify_loop(const List *list);
static void dump_list_data(const List *list, FILE *output);

#endif
