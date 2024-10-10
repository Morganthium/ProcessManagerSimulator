#include <stdio.h>
#include <stdlib.h>

#include "pagedmem.h"


//  traverses list from head to tail to insert node sorted by frame
void prependFrameSorted(list_t *list, node_t *newNode){
    int newFrame = *((int *)newNode->data);
    node_t *curNode = list->head;

    while (curNode && newFrame >= *((int *)curNode->data)){
        curNode = curNode->next;
    }
    insertNodeBefore(list, curNode, newNode);
}

//  traverses list from tail to head to insert node sorted by frame
void appendFrameSorted(list_t *list, node_t *newNode){
    int newFrame = *((int *)(newNode->data));

    node_t *curNode = list->tail;
    
    while (curNode && newFrame < *((int *)curNode->data)){
        curNode = curNode->prev;
    }
    insertNodeAfter(list, curNode, newNode);
}

//  allocate frames to a given process
void allocateFrames(list_t *availableFrames, process_t *currentProcess, int pagesReq){
    list_t *processMemory = (list_t *)currentProcess->memory;

    int count = 0;
    //  while the number of frames allocated is less than required
    while (count < pagesReq){
        //  move node from availableFrames to processMemory
        appendFrameSorted(processMemory, evictNodeFromList(availableFrames, availableFrames->head));
        count++;
    }
}

//  unassigns n frames of a process
void unassignProcessMemory(list_t *availableFrames, list_t *evictFrom, list_t *evicted, int n){
    node_t *curNode = evictFrom->head;
    node_t *nextNode;
        
    int count = 0;
    while(curNode && count < n){
        nextNode = curNode->next;

        evictNodeFromList(evictFrom, curNode);
        prependFrameSorted(availableFrames, curNode);
        if (evicted){
            int *ip = (int *)safeMalloc(sizeof(int));
            *ip = *((int *)curNode->data);
            node_t *copy = createNode(ip);
            prependFrameSorted(evicted, copy);
        }
            
        curNode = nextNode;
        count++;
    }
}


void freeMallocedInt(void *rawNum){
    int *num = (int *)rawNum;
    if (num){
        free(num);
    }
}

void printFrames(list_t *evicted){
    node_t *curNode = evicted->head;
    //  print frames
    while (curNode){
        fprintf(stdout, "%d", *((int *)curNode->data));
        if (curNode->next){
            fprintf(stdout, ",");
        }
        curNode = curNode->next;
    }
}

void printProcessEvicted(int time, list_t *evicted){
    fprintf(stdout, "%d,EVICTED,evicted-frames=[", time);
    printFrames(evicted);
    fprintf(stdout, "]\n");
}


//  prints process to stdout
void printProcessRunningPagedMem(int time, char *pname, int rtime, int musage, list_t *memory){
    fprintf(stdout, "%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[", time, pname, rtime, musage);
    printFrames(memory);
    fprintf(stdout, "]\n");
}


//  while not enough frames available in memory
void createSpaceInMemory(list_t * availableFrames, list_t *processes, int freeFramesNeeded, int time, int freeAll){
    if (processes->count == 0){
        return;
    }

    int numToFree;
    list_t *evicted = createList();
    node_t *curNode = processes->head;
    process_t *curProcess;

    //  while not enough frames have been freed 
    while (curNode && freeFramesNeeded > 0){
        curProcess = (process_t *)curNode->data;

        int framesInProcess = ((list_t *)(curProcess->memory))->count;
        numToFree = (freeAll == TRUE) ? framesInProcess : min(framesInProcess, freeFramesNeeded);
        unassignProcessMemory(availableFrames, (list_t *)curProcess->memory, evicted, numToFree);

        freeFramesNeeded -= numToFree;
        curNode = curNode->next;
    }
    printProcessEvicted(time, evicted);
    //  since nodes still need to exist, free only the list
    freeList(evicted, &freeMallocedInt);
}