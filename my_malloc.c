#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

static LinkList* my_memory = NULL;
static unsigned long data_segment_size = 0;
static unsigned long data_alloc_size = 0;

void* divide(LinkList* inLL, size_t size){
  LinkList* newNode = (LinkList*)(inLL->address + size);
  newNode->nextNode = inLL->nextNode;
  newNode->prevNode = inLL;
  newNode->size = inLL->size - (size + sizeof(LinkList));
  newNode->address = (char*)(newNode + 1);
  newNode->isFree = 1;
  if(inLL->nextNode != NULL){
    inLL->nextNode->prevNode = newNode;
  }
  inLL->nextNode = newNode;
  inLL->size = size;
  inLL->isFree = 0;
  data_alloc_size += size + sizeof(LinkList);
  return inLL->address;
}

void *ff_malloc(size_t size){
  LinkList* currNode = my_memory;
  if(currNode == NULL){
    void* temp = sbrk(size + sizeof(LinkList));
    LinkList* newNode = (LinkList*) temp;
    data_segment_size += size + sizeof(LinkList);
    data_alloc_size += size + sizeof(LinkList);
    newNode->nextNode = NULL;
    newNode->prevNode = NULL;
    newNode->size = size;
    newNode->address = (char*)(temp + sizeof(LinkList));
    newNode->isFree = 0;
    my_memory = newNode;
    return newNode->address;
  }
  while(currNode->nextNode){
    if(currNode->size < size && currNode->size != 0){
      currNode = currNode->nextNode;
    }
    else if(currNode->size >= size + sizeof(LinkList)){
      void* ans = divide(currNode, size);
      return ans;
    }
    else if(currNode->size >= size && currNode->size < size + sizeof(LinkList)){
      currNode->isFree = 0;
      data_alloc_size += size + sizeof(LinkList);
      return currNode->address;
    }
  }
  if (currNode->nextNode == NULL) {
    void* temp = sbrk(size + sizeof(LinkList));
    LinkList* newNode = (LinkList*) temp;
    data_segment_size += size + sizeof(LinkList);
    data_alloc_size += size + sizeof(LinkList);
    newNode->nextNode = NULL;
    newNode->prevNode = currNode;
    newNode->size = size;
    newNode->address = (char*)(temp + sizeof(LinkList));
    newNode->isFree = 0;
    return newNode->address;
  }
}

void conquer(LinkList* currNode){
  if(currNode->nextNode == NULL){
    return;
  }
  if(currNode->isFree && currNode->nextNode->isFree){
    currNode->size += currNode->nextNode->size + sizeof(LinkList);
    currNode->nextNode = currNode->nextNode->nextNode;
  }
  conquer(currNode->nextNode);
}

void ff_free(void *ptr){
  conquer(my_memory);
  LinkList* currNode = (LinkList*)(ptr - 1);
  currNode->isFree = 1;
  data_alloc_size -= currNode->size + sizeof(LinkList);
}

void *bf_malloc(size_t size){
  LinkList* minNode = NULL;
  size_t minSize = __SIZE_MAX__;
  LinkList* currNode = my_memory;
  while(currNode){
    if(currNode->size >= size + sizeof(LinkList) && currNode->size < minSize){
      minSize = currNode->size;
      minNode = currNode;
    }
    currNode = currNode->nextNode;
  }
  currNode = minNode;
  if(currNode == NULL){
    void* temp = sbrk(size + sizeof(LinkList));
    LinkList* newNode = (LinkList*) temp;
    data_segment_size += size + sizeof(LinkList);
    data_alloc_size += size + sizeof(LinkList);
    newNode->nextNode = NULL;
    newNode->prevNode = NULL;
    newNode->size = size;
    newNode->address = (char*)(temp + sizeof(LinkList));
    newNode->isFree = 0;
    my_memory = newNode;
    return newNode->address;
  }
  else{
    void* ans = divide(currNode, size);
    return ans;
  }
}

void bf_free(void *ptr){
  ff_free(ptr);
}

unsigned long get_data_segment_size(){
  return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
  return (data_segment_size - data_alloc_size);
} //in bytes
