#ifndef MY_MALLOC_H
#define MY_MALLOC_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
typedef struct _LinkList{
    struct _LinkList* prevNode;
    struct _LinkList* nextNode;
    size_t size;
    int isFree;
    char* address;
}LinkList;

//First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);

//Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in bytes

#endif
