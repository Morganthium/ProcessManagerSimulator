#ifndef _PAGED_MEM_H_
#define _PAGED_MEM_H_

#include "utils.h"

#define MIN_PAGES_ALLOC 4

void prependFrameSorted(list_t *list, node_t *newNode);
void appendFrameSorted(list_t *list, node_t *newNode);
void allocateFrames(list_t *availableFrames, process_t *currentProcess, int pagesReq);
void unassignProcessMemory(list_t *availableFrames, list_t *evictFrom, list_t *evicted, int n);
void freeMallocedInt(void *rawNum);

void printFrames(list_t *evicted);
void printProcessEvicted(int time, list_t *evicted);
void printProcessRunningPagedMem(int time, char *pname, int rtime, int musage, list_t *memory);

void createSpaceInMemory(list_t * availableFrames, list_t *processes, int freeFramesNeeded, int time, int freeAll);
#endif