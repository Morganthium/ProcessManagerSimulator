#include <stdio.h>
#include <stdlib.h>

#include "taskone.h"


void runTaskOne(list_t *processes, int quantum) {
    int time = 0;

    list_t *queue = createList();
    node_t *currentNode = NULL;
    process_t *currentProcess = NULL;
    int processCount = processes->count;
    double totalTurnaround = 0;
    double totalOverhead = 0;
    double maxOverhead = -1;

    enqueueArrivedProcesses(processes, queue, time);
    //  while there are still processes to be run
    while (!listEmpty(processes) || !listEmpty(queue)){
        //  run the process at the head of the queue
        if (!listEmpty(queue)){
            currentNode = evictNodeFromList(queue, queue->head);
            currentProcess = (process_t *)(currentNode->data);
            //  select the head of the queue to use the CPU for the duration of a quantum
            if (currentProcess->state == READY){
                currentProcess->state = RUNNING;
                printProcessRunningOne(time, currentProcess->processName, currentProcess->remainingTime);
            }

            currentProcess->remainingTime -= quantum;
            time += quantum;

            if (currentProcess->remainingTime <= 0){
                currentProcess->state = FINISHED;
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
}

//  prints process to stdout
void printProcessRunningOne(int time, char *pname, int rtime){
    fprintf(stdout, "%d,RUNNING,process-name=%s,remaining-time=%d\n", time, pname, rtime);
}

