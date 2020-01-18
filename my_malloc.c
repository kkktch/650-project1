#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

LinkList* my_memory;

void* divide(LinkList* inLL, size_t size){
    LinkList* newNode = inLL->address + size;
    newNode->nextNode = inLL->nextNode;
    newNode->prevNode = inLL->prevNode;
    newNode->size = inLL->size - size;
    newNode->address = newNode;
    if(inLL->prevNode != NULL){
        inLL->prevNode->nextNode = newNode;
    }
    if(inLL->nextNode != NULL){
        inLL->nextNode->prevNode = newNode;
    }
    inLL->nextNode = NULL;
    inLL->prevNode = NULL;
    inLL->size = size;
    return newNode->address;
}

void *ff_malloc(size_t size){
    LinkList* currNode = my_memory;
    while(currNode){
        if(currNode->size < size && currNode->size != 0){
            currNode = currNode->nextNode;
        }
        else if(currNode->size >= size){
            void* ans = divide(currNode, size);
            return ans;
        }
    }
    if(currNode == NULL){
        LinkList* newNode = (LinkList*)sbrk(size);;
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = newNode;
        return newNode->address;
    }
}
