# List
Realisation of cash-friendly circular doubly linked list.

## Functions using

* Constructor
```c++
int list_ctr(List* list, size_t list_size)
```
Macro over the real constructor with creation info addition. Construsts list of given size and returns errors occured during construction.

* Destructor
```c++
int list_dtor(List *list)
```
Destructs list by freeing memory allocated for data storage and filling all information with nulls and nullptrs. Returns errors occured during destruction.

* Insert
```c++
size_t list_insert(List *list, Elem_t elem, size_t position)
```
Inserts element after element from given position. Returns index of new position of inserted element. 

Attention! Most of the functions take the real index of element in list, not the logical index. It's your responsibility not to loose real indexes.

There are two functions over this one:

```c++
size_t list_insert_head(List *list, Elem_t elem)
```
Inserts element to the first logical position of list.

```c++
size_t list_insert_back(List *list, Elem_t elem)
```
Inserts element to the last logical position of list.

* Pop
```c++
Elem_t list_pop(List *list, size_t position)
```

Pops element placed in given position from list and returns its value. There are two functions over this one:

```c++
Elem_t list_pop_head(List *list)
```
Pops element from the first logical position. Useful if you want to use list as queue.

```c++
Elem_t list_pop_back(List *list)
```
Pops the last (logical) element from list. Useful if you want to use list as stack.

* Verificator
```c++
int list_verificator(const List *list)
```
Returns byte sum of errors founded in list. For normal list returns 0. Errors codes:
```c++
NO_LIST_ERRORS  = 0,
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
CANT_OPEN_FILE  = 1 << 10,
CANT_GENER_PIC  = 1 << 11,
BUSY_EXC_SIZE   = 1 << 12,
```

* Resizes
```c++
int resize_list_without_sort(List *list, size_t new_size)
```
Resizes list without sorting to given size. If size is less than amount of free cells at the end, list will be resized to the min size that is ok for resize.
```c++
int resize_list_with_sort(List *list, size_t new_size)
```
Resizes list with sorting (but works slover than without sorting, time is linear by amount of busy elements). Can resize list to every size that is less than size_t maximum value and more (or equal) than amount of busy elements.

* Getting indexes
```c++
size_t list_get_next(const List *list, size_t index);
```
```c++
size_t list_get_prev(const List *list, size_t index);
```
Returns physical indexes of next and previous element by its physical index.

```c++
size_t get_real_index_by_logical(List *list, size_t logical_index, int *errors = nullptr)
```
Returns real index of element by its logical index. Is not recommended to use because of long execution time (linear by logical index) that is not better than reallocation in array. Can also return errors occured while execution by using pointer to int var stores errors.

* Dump
```c++
#define dump_list(list)
```
Calls by macro wrap, that helps print information about place where dump was called. You need to open logfile by yourself and set it as logstream using this function:
```c++
SetLogStream(FILE *logstream)
```
It can be text file or html file. Additionally dump makes graph visualisation of list data with relations between elements. If you use text file, there will be information where you can find png picture. For html output define 
```c++
#define LOGS_TO_HTML
```
to have this pictures exactly in file.
