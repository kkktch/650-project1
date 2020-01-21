#include "my_malloc.h"
#include <assert.h>
#include <limits.h>
#include <unistd.h>

static LinkList *HeadNode = NULL;
static LinkList *TailNode = NULL;
static unsigned long data_segment_size = 0;
static unsigned long data_allocate_size = 0;
static size_t LLSIZE = sizeof(LinkList);

void eraseNode(LinkList* currNode);
void* deleteNode(LinkList* currNode);
void* divide(LinkList* currNode, size_t size);
void Traverse(LinkList* Node, int type);
void insertNode(LinkList* Node, int type);
void addNode(LinkList* Node);
void conquer(LinkList* currNode);
void conquerPrev(LinkList* currNode);
void conquerNext(LinkList* currNode);

void* ff_malloc(size_t size) {
    LinkList *currNode = HeadNode;
    while (currNode != NULL) {
        if (currNode->size < size) {
            currNode = currNode->nextNode;
        }
        else {
            if (currNode->size < size + LLSIZE) {
                void* ans = deleteNode(currNode);
                return ans;
            }
            else {
                void* ans = divide(currNode, size);
                return ans;
            }
        }
    }
    if (currNode == NULL) {
        void *tmp = sbrk(size + LLSIZE);
        LinkList *Node = tmp;
        data_segment_size += size + LLSIZE;
        data_allocate_size += size + LLSIZE;
        eraseNode(Node);
        Node->size = size;
        Node->address = tmp + LLSIZE;
        return Node->address;
    }
}

void ff_free(void *ptr) {
    LinkList *Node = (LinkList *)(ptr - LLSIZE);
    data_allocate_size -= Node->size + LLSIZE;
    conquer(Node);
}

void* bf_malloc(size_t size) {
    size_t minSize = __SIZE_MAX__;
    LinkList *currNode = HeadNode;
    LinkList *minNode = NULL;
    while (currNode) {
        if (currNode->size >= size && currNode->size < minSize) {
            minSize = currNode->size;
            minNode = currNode;
        }
        currNode = currNode->nextNode;
    }
    if (minNode == NULL) {
        void *tmp = sbrk(size + LLSIZE);
        data_segment_size += size + LLSIZE;
        data_allocate_size += size + LLSIZE;
        LinkList *Node = (LinkList *)tmp;
        eraseNode(Node);
        Node->size = size;
        Node->address = tmp + LLSIZE;
        return Node->address;
    }
    else {
        if (minNode->size < size + LLSIZE) {
            void* ans = deleteNode(minNode);
            return ans;
        }
        else {
            void* ans = divide(minNode, size);
            return ans;
        }
    }
}

void bf_free(void *ptr) {
    LinkList *Node = (LinkList *)(ptr - LLSIZE);
    data_allocate_size -= Node->size + LLSIZE;
    conquer(Node);
}


unsigned long get_data_segment_size() {
    return data_segment_size;
}
unsigned long get_data_segment_free_space_size() {
    unsigned long data_segment_free_space_size = data_segment_size - data_allocate_size;
    return data_segment_free_space_size;
}

void eraseNode(LinkList* currNode){
    currNode->nextNode = NULL;
    currNode->prevNode = NULL;
    currNode->isFree = 0;
}

void* deleteNode(LinkList* currNode){
    HeadNode = (currNode->prevNode == NULL) ? currNode->nextNode :
    (currNode->nextNode == NULL && currNode->prevNode == NULL) ? NULL : HeadNode;
    TailNode = (currNode->nextNode == NULL) ? currNode->prevNode :
    (currNode->nextNode == NULL && currNode->prevNode == NULL) ? NULL : TailNode;
    if (currNode->nextNode == NULL){
        currNode->prevNode->nextNode = NULL;
    }
    else if (currNode->prevNode == NULL){
        currNode->nextNode->prevNode = NULL;
    }
    else {
        currNode->prevNode->nextNode = currNode->nextNode;
        currNode->nextNode->prevNode = currNode->prevNode;
    }
    eraseNode(currNode);
    data_allocate_size += currNode->size + LLSIZE;
    return currNode->address;
    
}

void* divide(LinkList* currNode, size_t size){
    LinkList *newNode = (LinkList *)(currNode->address + size);
    data_allocate_size += size + LLSIZE;
    newNode->nextNode = currNode->nextNode;
    newNode->prevNode = currNode->prevNode;
    newNode->address = newNode + 1;
    newNode->size = currNode->size - (size + LLSIZE);
    newNode->isFree = 1;
    HeadNode = (currNode->prevNode == NULL) ? newNode : HeadNode;
    TailNode = (currNode->nextNode == NULL) ? newNode : TailNode;
    if (currNode->prevNode != NULL) {
        currNode->prevNode->nextNode = newNode;
    }
    if (currNode->nextNode != NULL) {
        currNode->nextNode->prevNode = newNode;
    }
    currNode->size = size;
    eraseNode(currNode);
    return currNode->address;
}

void Traverse(LinkList* Node, int type){
    LinkList* currNode = NULL;
    if (type > 0) {
        currNode = HeadNode;
        while (currNode != TailNode && currNode->nextNode < Node) {
            currNode = currNode->nextNode;
        }
    }
    else {
        currNode = TailNode;
        while (currNode != HeadNode && currNode->prevNode > Node) {
            currNode = currNode->prevNode;
        }
    }
    LinkList *tempNext = currNode->nextNode;
    LinkList *tempPrev = currNode->prevNode;
    if (type > 0) {
        tempNext->prevNode = Node;
        currNode->nextNode = Node;
    }
    else {
        tempPrev->nextNode = Node;
        currNode->prevNode = Node;
    }
    Node->nextNode = (type > 0) ? tempNext : currNode;
    Node->prevNode = (type > 0) ? currNode : tempPrev;
}

void insertNode(LinkList* Node, int type){
    if (type == 1) {
        HeadNode->prevNode = Node;
        Node->nextNode = HeadNode;
        Node->prevNode = NULL;
        HeadNode = Node;
    }
    if (type == 2) {
        TailNode->nextNode = Node;
        Node->prevNode = TailNode;
        Node->nextNode = NULL;
        TailNode = Node;
    }
    if (type == 3) {
        int distance = (int)HeadNode + (int)TailNode - 2 * (int)Node;
        Traverse(Node, distance);
    }
}

void addNode(LinkList *Node) {
    if (HeadNode == NULL && TailNode == NULL) {
        HeadNode = Node;
        TailNode = Node;
        return;
    }
    int type = (HeadNode > Node) ? 1 : (TailNode < Node) ? 2 : 3;
    insertNode(Node, type);
}

void conquerPrev(LinkList* currNode){
    currNode->prevNode->nextNode = currNode->nextNode;
    currNode->prevNode->size += currNode->size + LLSIZE;
    if (currNode->nextNode == NULL) {
        TailNode = currNode->prevNode;
    }
    else{
        currNode->nextNode->prevNode = currNode->prevNode;
        currNode->prevNode->nextNode = currNode->nextNode;
    }
}

void conquerNext(LinkList* currNode){
    currNode->size += currNode->nextNode->size + LLSIZE;
    if (currNode->nextNode->nextNode == NULL) {
        currNode->nextNode = NULL;
        TailNode = currNode;
    }
    else {
        currNode->nextNode->prevNode = NULL;
        currNode->nextNode = currNode->nextNode->nextNode;
        currNode->nextNode->prevNode = currNode;
    }
}

void conquer(LinkList* currNode) {
    addNode(currNode);
    if (currNode != HeadNode && currNode->prevNode->size == (char*)currNode - currNode->prevNode->address) {
        conquerPrev(currNode);
        currNode = currNode->prevNode;
    }
    if (currNode != TailNode && currNode->size == (char*)currNode->nextNode - currNode->address) {
        conquerNext(currNode);
    }
}
