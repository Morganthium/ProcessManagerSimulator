#ifndef _TASK_TWO_H_
#define _TASK_TWO_H_

#include "utils.h"

typedef struct memory memory_t;
struct memory {
    int address;
    int size;
};

void runTaskTwo(list_t *processes, int quantum);
void printProcessRunningTwo(int time, char *pname, int rtime, int musage, int addrs);
void freeMemory(void *rawMemory);
memory_t *createMemory(int address, int size);
int allocateMemoryFirstFit(list_t *memory, process_t *process);
int checkGap(int left, int right, int spaceNeeded);

#endif