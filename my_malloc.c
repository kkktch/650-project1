#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

static LinkList* my_memory_head = NULL;
static LinkList* my_memory_tail = NULL;
static unsigned long data_segment_size = 0;
static unsigned long data_alloc_size = 0;

void* divide(LinkList* inLL, size_t size){
    LinkList* newNode = (LinkList*)(inLL->address + size);
    newNode->nextNode = inLL->nextNode;
    newNode->prevNode = inLL->prevNode;
    newNode->size = inLL->size - (size + sizeof(LinkList));
    newNode->address = (char*)(newNode + 1);
    inLL->nextNode = NULL;
//    my_memory_head = (my_memory_head == inLL) ? newNode : my_memory_head;
//    my_memory_tail = (my_memory_tail == inLL) ? newNode : my_memory_tail;
    if (my_memory_head != inLL) {
        inLL->prevNode->nextNode = newNode;
    }
    else{
        my_memory_head = newNode;
    }
    if (my_memory_tail != inLL) {
        inLL->nextNode->prevNode = newNode;
    }
    else{
        my_memory_tail = newNode;
    }
    inLL->size = size;
    inLL->prevNode = NULL;
    data_alloc_size += size + sizeof(LinkList);
    return inLL->address;
}

void *ff_malloc(size_t size){
    LinkList* currNode = my_memory_head;
    while(currNode != NULL){
        if(currNode->size < size){
            currNode = currNode->nextNode;
            continue;
        }
        else if(currNode->size >= size + sizeof(LinkList)){
            void* ans = divide(currNode, size);
            return ans;
        }
        else if(currNode->size >= size && currNode->size < size + sizeof(LinkList)){
            if(my_memory_tail == my_memory_head && my_memory_head == currNode){
                my_memory_tail = NULL;
                my_memory_head = NULL;
            }
            else if(my_memory_head == currNode){
                currNode->nextNode->prevNode = NULL;
                my_memory_head = currNode->nextNode;
            }
            else if(my_memory_tail == currNode){
                currNode->prevNode->nextNode = NULL;
                my_memory_tail = currNode->prevNode;
            }
            else{
                currNode->prevNode->nextNode = currNode->nextNode;
                currNode->nextNode->prevNode = currNode->prevNode;
            }
            currNode->prevNode = NULL;
            currNode->nextNode = NULL;
            data_alloc_size += currNode->size + sizeof(LinkList);
            return currNode->address;
        }
    }
    
    if (currNode == NULL) {
        void* temp = sbrk(size + sizeof(LinkList));
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        LinkList* newNode = (LinkList*) temp;
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = temp + sizeof(LinkList);
        return newNode->address;
    }
}

void insert(LinkList* currNode){
    if(my_memory_head == NULL && my_memory_tail == NULL){
        my_memory_head = currNode;
        my_memory_tail = currNode;
    }
    else if (my_memory_head > currNode) {
        my_memory_head->prevNode = currNode;
        currNode->nextNode = my_memory_head;
        currNode->prevNode = NULL;
        my_memory_head = currNode;
    }
    else if (currNode > my_memory_tail) {
        my_memory_tail->nextNode = currNode;
        currNode->prevNode = my_memory_tail;
        currNode->nextNode = NULL;
        my_memory_tail = currNode;
    }
    else{
        if(currNode - my_memory_head < my_memory_tail - currNode){
            LinkList* temp = my_memory_head;
            while (temp && temp->nextNode && temp->nextNode < currNode) {
                temp = temp->nextNode;
            }
            LinkList* next = temp->nextNode;
            next->prevNode = currNode;
            temp->nextNode = currNode;
            currNode->nextNode = next;
            currNode->prevNode = temp;
        }
        else{
            LinkList* temp = my_memory_tail;
            while (temp && temp->prevNode && temp->prevNode > currNode) {
                temp = temp->prevNode;
            }
            LinkList* prev = temp->prevNode;
            prev->nextNode = currNode;
            temp->prevNode = currNode;
            currNode->nextNode = temp;
            currNode->prevNode = prev;
        }
    }
}

void conquer(LinkList* currNode){
    insert(currNode);
    if (currNode == NULL || currNode->nextNode == NULL || currNode->prevNode == NULL) {
        return;
    }
    if (currNode->nextNode && (long)currNode->size == (long)currNode->nextNode - (long)currNode->address) {
        currNode->size += currNode->nextNode->size + sizeof(LinkList);
        if (currNode->nextNode == my_memory_tail) {
            currNode->nextNode = NULL;
            my_memory_tail = currNode;
        }
        else{
            currNode->nextNode = currNode->nextNode->nextNode;
            currNode->nextNode->prevNode = currNode;
        }
    }
    if (currNode->prevNode && currNode->prevNode->size == (char*)currNode - currNode->prevNode->address) {
        currNode->prevNode->size += currNode->size + sizeof(LinkList);
        currNode->prevNode->nextNode = currNode->nextNode;
        if (my_memory_tail == currNode) {
            my_memory_tail = currNode->prevNode;
        }
        else {
            currNode->nextNode->prevNode = currNode->prevNode;
        }
        currNode = currNode->prevNode;
    }
}

void ff_free(void *ptr){
    LinkList* currNode = (LinkList*)((char*)ptr - sizeof(LinkList));
    data_alloc_size -= currNode->size + sizeof(LinkList);
    conquer(currNode);
}

void *bf_malloc(size_t size){
    LinkList* minNode = NULL;
    size_t minSize = __SIZE_MAX__;
    LinkList* currNode = my_memory_head;
    if (currNode == NULL) {
        void* temp = sbrk(size + sizeof(LinkList));
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        LinkList* newNode = (LinkList*) temp;
        newNode->nextNode = NULL;
        newNode->size = size;
        newNode->address = (char*)(temp + sizeof(LinkList));
        my_memory_head = newNode;
        return newNode->address;
    }
    while(currNode != NULL){
        if(currNode->isFree == 1 && currNode->size >= size && currNode->size < minSize){
            minSize = currNode->size;
            minNode = currNode;
        }
        currNode = currNode->nextNode;
    }
    if (minNode == NULL) {
        void* temp = sbrk(size + sizeof(LinkList));
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        LinkList* newNode = (LinkList*) temp;
        newNode->nextNode = NULL;
        newNode->size = size;
        newNode->address = (char*)(temp + sizeof(LinkList));
        newNode->isFree = 0;
        return newNode->address;
    }
    else if(minNode->size >= size + sizeof(LinkList)){
        void* ans = divide(minNode, size);
        return ans;
    }
    else if(minNode->size >= size && minNode->size < size + sizeof(LinkList)){
        minNode->isFree = 0;
        data_alloc_size += minNode->size + sizeof(LinkList);
        return minNode->address;
    }
}

void bf_free(void *ptr){
    LinkList* currNode = (LinkList*)(ptr - 1);
    currNode->isFree = 1;
    data_alloc_size -= currNode->size + sizeof(LinkList);
    conquer(my_memory_head);
}

unsigned long get_data_segment_size(){
    return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
    int data_segment_free_space_size = data_segment_size - data_alloc_size;
    return data_segment_free_space_size;
}
