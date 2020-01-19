#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // Library for sbrk()
#include "my_malloc.h" // my malloc header file

LinkList* my_memory;

unsigned long data_segment_size = 0;
unsigned long data_alloc_size = 0;

void* divide(LinkList* inLL, size_t size){
    LinkList* newNode = inLL->address + size;
    newNode->nextNode = inLL->nextNode;
    newNode->prevNode = inLL->prevNode;
    newNode->size = inLL->size - （size + sizeof(LinkList));
    newNode->address = newNode;
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
        data_segment_size += size + sizeof(LinkList);
        data_alloc_size += size + sizeof(LinkList);
        newNode->nextNode = NULL;
        newNode->prevNode = NULL;
        newNode->size = size;
        newNode->address = newNode + sizeof(LinkList);
        newNode->isFree = 0;
        return newNode->address;
    }
}


void addAndTraverseFromHead(meta *block) {
    meta *cur = head;
    while (cur != NULL && cur->next != NULL &&
        (unsigned long)cur->next < (unsigned long)block) {
        cur = cur->next;
    }
    assert(cur->next != NULL);
    meta *tempNext = cur->next;
    tempNext->prev = block;
    cur->next = block;
    block->next = tempNext;
    block->prev = cur;
}

void addAndTraverseFromTail(meta *block) {
    meta *cur = tail;
    while (cur != NULL && cur->prev != NULL &&
        (unsigned long)cur->prev > (unsigned long)block) {
        cur = cur->prev;
    }
    assert(cur->prev != NULL);
    meta *tempPrev = cur->prev;
    tempPrev->next = block;
    cur->prev = block;
    block->next = cur;
    block->prev = tempPrev;
}
void add(meta *block) {
    if (head == NULL && tail == NULL) {
        head = block;
        tail = block;
    }
    else if ((unsigned long)head > (unsigned long)block) {
        head->prev = block;
        block->next = head;
        block->prev = NULL;
        head = block;
    }
    else if ((unsigned long)tail < (unsigned long)block) {
        tail->next = block;
        block->prev = tail;
        block->next = NULL;
        tail = block;
    }
    else {
        unsigned long distToHead = (unsigned long)block - (unsigned long)head;
        unsigned long distToTail = (unsigned long)tail - (unsigned long)block;
        if (distToHead < distToTail) {
            addAndTraverseFromHead(block);
        }
        else {
            addAndTraverseFromTail(block);
        }
    }
}
void merge(meta *cur) {
    if (cur->prev != NULL) {
        meta *prevBlock = cur->prev;
        if (prevBlock->address + prevBlock->size == (char *)cur) {
            prevBlock->size += cur->size + sizeof(meta);
            prevBlock->next = cur->next;
            if (cur == tail) {
                tail = prevBlock;
            }
            else { // cur->next != NULL
                cur->next->prev = prevBlock;
            }
            cur = prevBlock;
        }
    }
    if (cur->next != NULL && cur->address + cur->size == (char *)cur->next) {
        cur->size += cur->next->size + sizeof(meta);
        if (cur->next == tail) {
            cur->next = NULL;
            tail = cur;
        }
        else {
            cur->next = cur->next->next;
            cur->next->prev = cur;
        }
    }
}

void ff_free(void *ptr) {
    meta *block = (meta *)((char *)ptr - sizeof(meta));
    allocated -= block->size + sizeof(meta);
    add(block);
    merge(block);
}



//void conquer(LinkList* node){
//    if(node->nextNode && node->nextNode->isFree){
//        node->size += node->nextNode->size + sizeof(LinkList);
//        node->nextNode = node->nextNode->nextNode;
//    }
//    if(node->prevNode && node->prevNode->isFree){
//        node->size += node->prevNode->size + sizeof(LinkList);
//        node->prevNode = node->prevNode->prevNode;
//    }
//}
//
//void ff_free(void *ptr){
//    LinkList* currNode = (LinkList*)(ptr - sizeof(LinkList));
//    data_alloc_size -= currNode->size + sizeof(LinkList);
//    LinkList* backNode = currNode;
//    while(currNode->nextNode){
//        conquer(currNode);
//        currNode = currNode->nextNode;
//    }
//    currNode = backNode;
//    while (currNode->prevNode) {
//        conquer(currNode);
//        currNode = currNode->prevNode;
//    }
//    currNode->isFree = 1;
//}

void *bf_malloc(size_t size){
    LinkList* minNode = NULL;
    size_t minSize = SIZE_MAX;
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
        newNode->address = newNode + sizeof(LinkList);
        newNode->isFree = 0;
        return newNode->address;
    }
    else{
        void* ans = divide(currNode, size);
        return ans;
    }
}

void bf_free(void *ptr){
    
}

unsigned long get_data_segment_size(){
    return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
    return (data_segment_size - data_alloc_size);
} //in bytes
