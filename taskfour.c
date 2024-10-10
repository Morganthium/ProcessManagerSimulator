#include <stdio.h>
#include <stdlib.h>

#include "taskfour.h"
#include "pagedmem.h"


void runTaskFour(list_t *processes, int quantum) {
    int time = 0;

    list_t *queue = createList();

    list_t *availableFrames = createList();
    int *ip = NULL;
    //  create a list of memory frames
    for (int i = 0; i < MAX_PAGES; i++){
        ip = (int *)safeMalloc(sizeof(int));
        *ip = i;
        appendNode(createNode(ip), availableFrames);
    }

    node_t *currentNode = NULL;
    process_t *currentProcess = NULL;
    list_t *processMemory = NULL;
    int processCount = processes->count;
    double totalTurnaround = 0;
    double totalOverhead = 0;
    double maxOverhead = -1;

    //  initialise all processes with lists 
    currentNode = processes->head;
    while (currentNode){
        currentProcess = (process_t *)(currentNode->data);
        currentProcess->memory = createList();
        currentNode = currentNode->next;
    }

    enqueueArrivedProcesses(processes, queue, time);
    //  while there are still processes to be run
    
    while (!listEmpty(processes) || !listEmpty(queue)){
        //  run the process at the head of the queue
        if (!listEmpty(queue)){
            currentNode = evictNodeFromList(queue, queue->head);
            currentProcess = (process_t *)(currentNode->data);
            processMemory = (list_t *)(currentProcess->memory);
            
            //  process does not have enough pages
            if (processMemory->count < min(currentProcess->pagesReq, MIN_PAGES_ALLOC)){
                int toEvict = 0;
                int toAllocate = min(currentProcess->pagesReq - processMemory->count, availableFrames->count);
                int pagesWithoutEviction = processMemory->count + toAllocate;
                //  if there are not enough pages from free memory
                if (pagesWithoutEviction < MIN_PAGES_ALLOC && pagesWithoutEviction < currentProcess->pagesReq){
                    toEvict = MIN_PAGES_ALLOC - toAllocate;
                    if (toEvict > 0){
                        createSpaceInMemory(availableFrames, queue, toEvict, time, FALSE);
                    }
                    else{
                        toEvict = 0;
                    }
                }
                allocateFrames(availableFrames, currentProcess, toAllocate + toEvict);
            }

            //  select the head of the queue to use the CPU for the duration of a quantum
            if (currentProcess->state == READY){
                currentProcess->state = RUNNING;
                printProcessRunningPagedMem(time, currentProcess->processName, currentProcess->remainingTime, calcPagedMemUsage(availableFrames->count), (list_t *)currentProcess->memory);
            }
            
            currentProcess->remainingTime -= quantum;
            time += quantum;

            //  the process has finished
            if (currentProcess->remainingTime <= 0){
                currentProcess->state = FINISHED;
                printProcessEvicted(time, (list_t *)currentProcess->memory);
                unassignProcessMemory(availableFrames, (list_t *)currentProcess->memory, NULL, ((list_t *)currentProcess->memory)->count);
                enqueueArrivedProcesses(processes, queue, time);
                printProcessFinished(time, currentProcess->processName, queue->count);
                
                //  add to performance statistics
                int turnaroundTime = time - currentProcess->arrivalTime;
                double overheadTime = (double)turnaroundTime / currentProcess->serviceTime;
                totalTurnaround += turnaroundTime;
                totalOverhead += overheadTime;
                if (overheadTime > maxOverhead){
                    maxOverhead = overheadTime;
                }
                free((list_t *)currentProcess->memory);
                freeNode(currentNode, &freeProcess);

                currentNode = NULL;
                currentProcess = NULL;
            }        
        }
        else {
            time += quantum;
        }

        //  add arrived processes to queue
        enqueueArrivedProcesses(processes, queue, time);
        // add previous process to queue
        if (currentNode){
            if (!listEmpty(queue)) {
                currentProcess->state = READY;
            }
            appendNode(currentNode, queue);
            currentNode = NULL;
            currentProcess = NULL;
        }
    }
    

    //  calculate avg turnaround
    totalTurnaround /= processCount;
    // round up
    int avgTurnaround = (int)totalTurnaround;
    avgTurnaround = avgTurnaround + (totalTurnaround > avgTurnaround ? 1 : 0);

    //  calculate avg overhead
    double avgOverhead = totalOverhead/processCount;

    printPerformanceStats(avgTurnaround, maxOverhead, avgOverhead, time);
    freeList(queue, &freeProcess);
    freeList(availableFrames, &freeMallocedInt);
}


