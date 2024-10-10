#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "utils.h"


//  safely allocates memory to a pointer of a given size
void *safeMalloc(size_t size) {
    void *ptr = malloc(size);
    assert(ptr);
    return ptr;
}

process_t *createProcess(int arrival, char *name, int service, short memreq){
    process_t *process = (process_t *)safeMalloc(sizeof(process_t));
    process->arrivalTime = arrival;
    process->processName = (char *)safeMalloc(sizeof(char) * (strlen(name) + 1));
    strcpy(process->processName, name);
    process->serviceTime = service;
    process->remainingTime = service;
    process->memoryRequirement = memreq;
    process->memory = NULL;
    process->state = -1;
    process->pagesReq = ((memreq + 3) & ~3)/4;
    return process;
}   

node_t *createNode(void *data){
    node_t *node = (node_t *)safeMalloc(sizeof(node_t));
    node->data = data;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

list_t *createList(){
    list_t *list = (list_t *)safeMalloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

//  frees a process, but not the memory stored
void freeProcess(void *rawProcess){
    process_t *process = (process_t *)rawProcess;
    if (process){
        if (process->processName){
            free(process->processName);
        }
        free(process);
    }
}



void freeNode(node_t *node, void (*freeFunction)(void *)){
    if (node){
        freeFunction(node->data);
        free(node);
    }
}

void freeList(list_t *list,  void (*freeFunction)(void *)){
    if (list){
        node_t *temp;
        while (list->head){
            temp = list->head;
            list->head = list->head->next;
            freeFunction(temp->data);
            free(temp);
        }
        free(list);
    }
}

void appendNode(node_t *node, list_t *list){
    //  empty list
    if (listEmpty(list)){
        list->count++;
        list->head = node;
        list->tail = node;
        return;
    }

    list->count++;
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
    node->next = NULL;
}

void prependNode(node_t *node, list_t *list){
    // empty list
    if (listEmpty(list)){
        list->count++;
        list->head = node;
        list->tail = node;
        return;
    }
    
    list->count++;
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
}

int listEmpty(list_t *list){
    if (list->count > 0){
        return 0;
    }
    return 1;
}

//  evicts a node from a list and returns it
node_t *evictNodeFromList(list_t *list, node_t *node){
    //  node not in a list
    assert(list->head && list->tail);

    list->count--;

    if (list->head == node){
        list->head = node->next;
    }
    if (list->tail == node){
        list->tail = node->prev;
    }

    node_t *before = node->prev;
    node_t *after = node->next;
    if (before){
        before->next = after;
    }
    if (after){
        after->prev = before;
    }

    node->prev = NULL;
    node->next = NULL;

    return node;
}

void printPerformanceStats(int turnaround, double maxOverhead, double avgOverhead, int makespan){
    fprintf(stdout, "Turnaround time %d\n", turnaround);
    fprintf(stdout, "Time overhead %.2f %.2f\n", round(maxOverhead * 100.0) / 100.0, round(avgOverhead * 100.0) / 100.0);
    fprintf(stdout, "Makespan %d\n", makespan);
}

//  moves arrived processes from processes list to end of queue list
void enqueueArrivedProcesses(list_t *processes, list_t *queue, int time){
    node_t *temp;
    while (!listEmpty(processes) && ((process_t *)processes->head->data)->arrivalTime <= time){
        temp = evictNodeFromList(processes, processes->head);
        appendNode(temp, queue);
        ((process_t *)temp->data)->state = READY;
    }
}

void printProcessFinished(int time, char *pname, int pleft){
    fprintf(stdout, "%d,FINISHED,process-name=%s,proc-remaining=%d\n", time, pname, pleft);
}

//  inserts a node into a list before a given node
void insertNodeBefore(list_t *list, node_t *next, node_t *newNode){
    if (next == NULL){
        appendNode(newNode, list);
        return;
    }
    
    list->count++;
    node_t *prev = next->prev;
    next->prev = newNode;
    
    newNode->prev = prev;
    newNode->next = next;

    if (prev != NULL){
        prev->next = newNode;
    }
    else if (list->head == next){
        list->head = newNode;
    }
}

//  inserts a node into a list after a given node
void insertNodeAfter(list_t *list, node_t *prev, node_t *newNode){
    if (prev == NULL){
        prependNode(newNode, list);
        return;
    }

    list->count++;
    node_t *next = prev->next;
    prev->next = newNode;
    
    newNode->prev = prev;
    newNode->next = next;

    if (next != NULL){
        next->prev = newNode;
    }
    else if (list->tail == prev){
        list->tail = newNode;
    }
}

//  calculate memory usage
int calcPagedMemUsage(int freePages){
    int occupiedPages = MAX_PAGES - freePages;
    double percent = ((double)occupiedPages / MAX_PAGES) * 100;
    return roundDoubleUp(percent);
}

//  calculate memory usage
int calcContiguousMemUsage(int usedKb){
    double percent = ((double)usedKb / TOTAL_MEM) * 100;
    return roundDoubleUp(percent);
}

//  rounds a double up to the nearest int
int roundDoubleUp(double num){
    int rounded = num;
    rounded = rounded + (num > rounded ? 1 : 0);
    return rounded;
}

int min(int a, int b){
    return a <= b ? a : b;
}

int max(int a, int b){
    return a >= b ? a : b;
}