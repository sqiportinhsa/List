#ifndef LIST
#define LIST

#include "Libs/logging.h"

#define LOGS_TO_HTML

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
    size_t        in_usage = 0;
    size_t       list_size = 0;
    size_t            free = 0;
} List;

typedef enum {
    NO_LIST_ERRORS      = 0,
    NULLPTR_TO_LIST     = 1 << 0,
    NULLPTR_TO_DATA     = 1 << 1,
    NULLPTR_TO_LOGS     = 1 << 2,
    NOT_ENOUGTH_MEM     = 1 << 3,
    POP_ZERO_ELEM       = 1 << 4,
    LIST_IS_FULL        = 1 << 5,
    POS_DONT_EXIST      = 1 << 6,
    NON_POISON_FREE     = 1 << 7,
    CANNOT_RESIZE       = 1 << 8,
    CANT_OPEN_FILE      = 1 << 9,
    CANT_GENER_PIC      = 1 << 10,
    USED_EXC_SIZE       = 1 << 11,
    BROKEN_IN_USE_LOOP  = 1 << 12,
    BROKEN__FREE__LOOP  = 1 << 13,
} List_errors;


static const char *RESERVED_FILL__COLOR = "#BEA0A0";
static const char *RESERVED_FRAME_COLOR = "#361C1C";

static const char *DATACELL_FILL__COLOR = "#9AA5BB";
static const char *DATACELL_FRAME_COLOR = "#232D42";

static const char *FREE_FILL__COLOR = "#E2D2B4";
static const char *FREE_FRAME_COLOR = "#423723";

static const char *PREV_ARROW_COLOR = "#54303c";
static const char *NEXT_ARROW_COLOR = "#303C54";

static const char *FREE_ARROW_COLOR = "#423723";


#define RETURN_IF(error) if (error)      \
                            return errors;

#define CHECK_FOR_NULLPTR(ptr, errors, error_name) if(ptr == nullptr)       \
                                                        errors |= error_name;

#define CHECK_FOR_NULLPTR_WITH_MESSAGE(ptr, errors, error_name, message) \
        if (ptr == nullptr) {                                            \
            errors |= error_name;                                        \
            PrintToLogs(message);                                        \
            return errors;                                               \
        }

#define list_ctr(list, list_size) real_list_ctr(list, list_size, __FILE__, __PRETTY_FUNCTION__, \
                                                                                        __LINE__)

#define dump_list(list, message, ...) real_dump_list(list, __FILE__, __PRETTY_FUNCTION__, __LINE__, message, ##__VA_ARGS__)

#define Print_code(format, ...)                    \
        fprintf(code_output, format, ##__VA_ARGS__);


#define Print_busy(list, i)                                                                           \
        Print_code("node%zu [label=\"%d|{p:%4zu|i:%4zu|n:%4zu}\",fillcolor=\"%s\",color=\"%s\"];\n",  \
                                     i, list->data[i].val, list->data[i].prev, i, list->data[i].next, \
                                                         DATACELL_FILL__COLOR, DATACELL_FRAME_COLOR);

#define Print_free(list, i)                                                                            \
        Print_code("node%zu [label=\"free|{p:psn|i:%4zu|n:%4zu}\",fillcolor=\"%s\",color=\"%s\"];\n",  \
                                        i, i, list->data[i].next, FREE_FILL__COLOR, FREE_FRAME_COLOR);

#define Print_reserved(list)                                                                              \
        Print_code("node0 [label=\"reserved|{p:%4zu|i:%4zu|n:%4zu}\",fillcolor=\"%s\",color=\"%s\"];\n",  \
                  list->data[0].prev, 0, list->data[0].next, RESERVED_FILL__COLOR, RESERVED_FRAME_COLOR); \

#define Print_in_use_next_arrow(list, i)                                                          \
        Print_code("node%zu->node%zu [color=\"%s\",constraint=false];\n", i, list->data[i].next,  \
                                                                              NEXT_ARROW_COLOR);

#define Print_in_use_prev_arrow(list, i)                                                          \
        Print_code("node%zu->node%zu [color=\"%s\",constraint=false];\n", i, list->data[i].prev,  \
                                                                              PREV_ARROW_COLOR);
                                                                              
#define Print_free__next__arrow(list, i)                                                          \
        Print_code("node%zu->node%zu [color=\"%s\",constraint=false];\n", i, list->data[i].next,  \
                                                                              FREE_ARROW_COLOR);

#define Print_connection__arrow(list, i)                                     \
        Print_code("node%zu->node%zu [style=invis, weight = 100]\n", i, i+1);
        

int real_list_ctr(List *list, size_t list_size, const char *file, const char *func, int line);
int list_dtor(List *list);

size_t list_insert(List *list, Elem_t elem, size_t position);

size_t list_insert_head(List *list, Elem_t elem);
size_t list_insert_back(List *list, Elem_t elem);

Elem_t list_pop(List *list, size_t position);

Elem_t list_pop_head(List *list);
Elem_t list_pop_back(List *list);

size_t list_get_next(const List *list, size_t index);
size_t list_get_prev(const List *list, size_t index);

int list_verificator(const List *list);

int real_dump_list(const List *list, const char* file, const char* func, int line, const char *message, ...);

int resize_list_without_sort(List *list, size_t new_size);
int resize_list_with_sort(List *list, size_t new_size);

size_t get_real_index_by_logical(List *list, size_t logical_index, int *errors = nullptr);

#endif
