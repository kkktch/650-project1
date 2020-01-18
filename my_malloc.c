#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

LinkList* my_memory;

void memory_init(LineList* src){
  src->currNode = NULL;
  src->prevNode = NULL;
  src->size = 0;
}

void *ff_malloc(size_t size){
  if(!(my_memory)){
    memory_init(my_memory);
  }
}
