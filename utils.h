#ifndef _UTILS_H_
#define _UTILS_H_

#define READY 0
#define RUNNING 1
#define FINISHED 2

#define MAX_PROCESS_NAME_LEN 8
#define BUFFER_SIZE 36
#define NUM_INPUT_COLS 4
#define SPACE " "

#define TRUE 1
#define FALSE 0

#define TOTAL_MEM 2048
#define MAX_PAGES 512 


typedef struct node node_t;
struct node {
    void *data;
    node_t *prev;
    node_t *next;
};

typedef struct list list_t;
struct list {
    node_t *head;
    node_t *tail;
    int count;
};


typedef struct process process_t;
struct process {
    int arrivalTime;
    char *processName;
    int serviceTime;
    short memoryRequirement;
    int remainingTime;
    int state;
    int pagesReq;
    void *memory;
};


void *safeMalloc(size_t size);

process_t *createProcess(int arrival, char *name, int service, short memreq);
node_t *createNode(void *data);
list_t *createList();

void freeProcess(void *process);
void freeList(list_t *list,  void (*freeFunction)(void *));
void freeNode(node_t *node, void (*freeFunction)(void *));

void appendNode(node_t *node, list_t *list);
void prependNode(node_t *node, list_t *list);

int listEmpty(list_t *list);
node_t *evictNodeFromList(list_t *list, node_t *node);
void enqueueArrivedProcesses(list_t *processes, list_t *queue, int time);

void printProcessFinished(int time, char *pname, int pleft);
void printPerformanceStats(int turnaround, double maxOverhead, double avgOverhead, int makespan);

void insertNodeBefore(list_t *list, node_t *next, node_t *newNode);
void insertNodeAfter(list_t *list, node_t *prev, node_t *newNode);

int calcPagedMemUsage(int freePages);
int calcContiguousMemUsage(int usedKb);
int roundDoubleUp(double num);
int min(int a, int b);
int max(int a, int b);

#endif