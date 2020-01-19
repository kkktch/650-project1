#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

LinkList* my_memory;

unsigned long data_segment_size = 0;
unsigned long data_segment_free_space_size = 0;

void* divide(LinkList* inLL, size_t size){
    LinkList* newNode = inLL->address + size;
    newNode->nextNode = inLL->nextNode;
    newNode->prevNode = inLL->prevNode;
    newNode->size = inLL->size - size;
    newNode->address = newNode;
    newNode->isFree = 1;
    if(inLL->prevNode != NULL){
        inLL->prevNode->nextNode = newNode;
    }
    if(inLL->nextNode != NULL){
        inLL->nextNode->prevNode = newNode;
    }
    inLL->nextNode = NULL;
    inLL->prevNode = NULL;
    inLL->size = size;
    inLL->isFree = 0;
    data_segment_free_space_size += size + sizeof(LinkList);
    return newNode->address;
}

void *ff_malloc(size_t size){
    LinkList* currNode = my_memory;
    while(currNode){
        if(currNode->size < size && currNode->size != 0){
            currNode = currNode->nextNode;
        }
        else if(currNode->size >= size + sizeof(LinkList)){
            void* ans = divide(currNode, size + sizeof(LinkList));
            return ans;
        }
    }
    if(currNode == NULL){
        LinkList* newNode = (LinkList*)sbrk(size + sizeof(LinkList));
        data_segment_size += size + sizeof(LinkList);
        data_segment_free_space_size += size + sizeof(LinkList);
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = newNode + sizeof(LinkList);
        newNode->isFree = 0;
        return newNode->address;
    }
}

void conquer(LinkList* node){
    if(node->nextNode && node->nextNode->isFree){
        node->size += node->nextNode->size;
        node->nextNode = node->nextNode->nextNode;
    }
    if(node->prevNode && node->prevNode->isFree){
        node->size += node->prevNode->size;
        node->prevNode = node->prevNode->prevNode;
    }
}

void ff_free(void *ptr){
    LinkList* currNode = (LinkList*)(ptr - sizeof(LinkList));
    currNode->isFree = 1;
    LinkList* backNode = currNode;
    while(currNode->nextNode){
        conquer(currNode);
        currNode = currNode->nextNode;
    }
    currNode = backNode;
    while (currNode->prevNode) {
        conquer(currNode);
        currNode = currNode->prevNode;
    }
}

unsigned long get_data_segment_size(){
    return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
    return data_segment_size - data_segment_free_space_size;
} //in bytes
