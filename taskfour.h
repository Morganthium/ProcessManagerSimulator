#ifndef _TASK_FOUR_H_
#define _TASK_FOUR_H_

#include "utils.h"

void runTaskFour(list_t *processes, int quantum);
void createMinimumSpaceInMemory(list_t * availableFrames, list_t *processes, int freeFramesNeeded, int time);



#endif