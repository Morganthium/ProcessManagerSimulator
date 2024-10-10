#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tasktwo.h"
#include "utils.h"

void runTaskTwo(list_t *processes, int quantum) {
    int time = 0;

    list_t *queue = createList();
    list_t *memory = createList();
    node_t *currentNode = NULL;
    process_t *currentProcess = NULL;
    int processCount = processes->count;
    double totalTurnaround = 0;
    double totalOverhead = 0;
    double maxOverhead = -1;
    int usedKb = 0;

    enqueueArrivedProcesses(processes, queue, time);
    //  while there are still processes to be run
    while (!listEmpty(processes) || !listEmpty(queue)){
        //  run the process at the head of the queue
        if (!listEmpty(queue)){
            currentNode = queue->head;
            currentNode = evictNodeFromList(queue, queue->head);
            currentProcess = (process_t *)(currentNode->data);

            node_t *firstNode = currentNode;
            int kbAlloced = 0;
            if (currentProcess->memory == NULL){
                kbAlloced = allocateMemoryFirstFit(memory, currentProcess);
                //  try to allocate memory to a process
                while (kbAlloced <= 0 && currentProcess->memory == NULL){
                    if (currentNode){
                        appendNode(currentNode, queue);
                    }
                    currentNode = evictNodeFromList(queue, queue->head);
                    currentProcess = (process_t *)(currentNode->data);

                    //  back to the first process
                    if (currentNode == firstNode){
                        break;
                    }
                    kbAlloced = allocateMemoryFirstFit(memory, currentProcess);
                }

            }
            //  no processes that can be allocated memory
            if (currentProcess->memory == NULL){
                continue;
            }
            usedKb += kbAlloced;
            
            
            //  select the head of the queue to use the CPU for the duration of a quantum
            memory_t *memBlock = (memory_t *)((node_t *)currentProcess->memory)->data;
            if (currentProcess->state == READY){
                currentProcess->state = RUNNING;
                printProcessRunningTwo(time, currentProcess->processName, currentProcess->remainingTime, calcContiguousMemUsage(usedKb), memBlock->address);
            }


            currentProcess->remainingTime -= quantum;
            time += quantum;

            if (currentProcess->remainingTime <= 0){
                currentProcess->state = FINISHED;
                usedKb -= currentProcess->memoryRequirement;
                //  add arrived processes to queue
                enqueueArrivedProcesses(processes, queue, time);
                printProcessFinished(time, currentProcess->processName, queue->count);

                node_t *memNode = evictNodeFromList(memory, ((node_t *)currentProcess->memory));
                freeNode(memNode, &freeMemory);
                currentProcess->memory = NULL;
                
                //  add to performance statistics
                int turnaroundTime = time - currentProcess->arrivalTime;
                double overheadTime = (double)turnaroundTime / currentProcess->serviceTime;
                totalTurnaround += turnaroundTime;
                totalOverhead += overheadTime;
                if (overheadTime > maxOverhead){
                    maxOverhead = overheadTime;
                }
                freeNode(currentNode, &freeProcess);

                currentNode = NULL;
                currentProcess = NULL;
            }
        }
        else {
            time += quantum;
        }

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
    avgTurnaround = avgTurnaround + (totalTurnaround > avgTurnaround? 1 : 0);

    //  calculate avg overhead
    double avgOverhead = totalOverhead/processCount;

    printPerformanceStats(avgTurnaround, maxOverhead, avgOverhead, time);
    freeList(queue, &freeProcess);
    freeList(memory, &freeMemory);
}

//  prints process to stdout
void printProcessRunningTwo(int time, char *pname, int rtime, int musage, int addrs){
    fprintf(stdout, "%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%d\n", time, pname, rtime, musage, addrs);
}

//  tries to allocate a process memory, returns the amount of memory allocated
int allocateMemoryFirstFit(list_t *memory, process_t *process){
    memory_t *memBlock;
    node_t *currentNode = memory->head;
    node_t *newMem;

    int gapStart = 0;
    int gapEnd = (currentNode == NULL) ? TOTAL_MEM : ((memory_t *)currentNode->data)->address;
    

    //  if list is empty or enough memory at start of list
    if (listEmpty(memory) || checkGap(gapStart, gapEnd, process->memoryRequirement)){
        memBlock = createMemory(gapStart, process->memoryRequirement);
        newMem = createNode(memBlock);
        prependNode(newMem, memory);
        process->memory = newMem;
        return process->memoryRequirement;
    }

    node_t *nextNode;
    //  check gaps between nodes
    while (currentNode != NULL){
        nextNode = currentNode->next;

        gapStart = ((memory_t *)currentNode->data)->address + ((memory_t *)currentNode->data)->size;
        gapEnd = (nextNode == NULL) ? TOTAL_MEM : ((memory_t *)nextNode->data)->address;
 
        //  space found
        if (checkGap(gapStart, gapEnd, process->memoryRequirement)){
            memBlock = createMemory(gapStart, process->memoryRequirement);
            newMem = createNode(memBlock);
            insertNodeAfter(memory, currentNode, newMem);
            process->memory = newMem;
            return process->memoryRequirement;
        }

        currentNode = currentNode->next;
    }
    return 0;

}

//  checks whether a gap is large enough for the space needed
int checkGap(int left, int right, int spaceNeeded){
    return (right - left) >= spaceNeeded ? 1 :0;
}

memory_t *createMemory(int address, int size){
    memory_t *memory = (memory_t *)safeMalloc(sizeof(memory_t));
    memory->address = address;
    memory->size = size;
    return memory;
}

void freeMemory(void *rawMemory){
    memory_t *memory = (memory_t *)rawMemory;
    if (memory){
        free(memory);
    }
}
