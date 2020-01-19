#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

LinkList* my_memory = NULL;
unsigned long data_segment_size = 0;
unsigned long data_alloc_size = 0;

void* divide(LinkList* inLL, size_t size){
    LinkList* newNode = inLL->address + size;
    newNode->nextNode = inLL->nextNode;
    newNode->prevNode = inLL->prevNode;
    newNode->size = inLL->size - (size + sizeof(LinkList));
    newNode->address = (void*)(newNode + 1);
    newNode->isFree = 1;
    if(inLL->prevNode != NULL){
        inLL->prevNode->nextNode = newNode;
    }
    else if(inLL->prevNode == NULL){
        my_memory = newNode;
    }
    if(inLL->nextNode != NULL){
        inLL->nextNode->prevNode = newNode;
    }
    inLL->nextNode = NULL;
    inLL->prevNode = NULL;
    inLL->size = size;
    inLL->isFree = 0;
    data_alloc_size += size + sizeof(LinkList);
    return newNode->address;
}

void *ff_malloc(size_t size){
    LinkList* currNode = my_memory;
    while(currNode){
        if(currNode->size < size + sizeof(LinkList) && currNode->size != 0){
            currNode = currNode->nextNode;
        }
        else if(currNode->size >= size + sizeof(LinkList)){
            void* ans = divide(currNode, size);
            return ans;
        }
    }
    if(currNode == NULL){
        LinkList* newNode = (LinkList*)sbrk(size + sizeof(LinkList));
        my_memory = newNode;
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = (void*)(newNode + sizeof(LinkList));
        newNode->isFree = 0;
        return newNode->address;
    }
}

void conquer(LinkList* node){
    if(node->nextNode && node->nextNode->isFree){
        node->size += node->nextNode->size + sizeof(LinkList);
        node->nextNode = node->nextNode->nextNode;
        conquer(node->nextNode);
    }
    if(node->prevNode && node->prevNode->isFree){
        node->size += node->prevNode->size + sizeof(LinkList);
        node->prevNode = node->prevNode->prevNode;
        conquer(node->prevNode);
    }
}

void ff_free(void *ptr){
    LinkList* currNode = (LinkList*)(ptr - sizeof(LinkList));
    currNode->isFree = 1;
    data_alloc_size -= currNode->size + sizeof(LinkList);
    LinkList* backNode = currNode;
    conquer(currNode);
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
        LinkList* newNode = (LinkList*)sbrk(size + sizeof(LinkList));
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = (void*)(newNode + sizeof(LinkList));
        newNode->isFree = 0;
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
