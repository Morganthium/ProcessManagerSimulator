/*
    Project 1 - Process and Memory Management
    Computer Systems | COMP30023
    The University of Melbourne

    Victoria Lyngaae | 1350334
    Tarish Kadam | 1141070
*/

// --------------------------------------------------------------- //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "taskone.h"
#include "tasktwo.h"
#include "taskthree.h"
#include "taskfour.h"
#include "utils.h"

#define DASH '-'
#define FILE_FLAG 'f'
#define MEM_FLAG 'm'
#define QUANTUM_FLAG 'q'
#define INFINITE "infinite"
#define FIRST_FIT "first-fit"
#define PAGED "paged"
#define VIRTUAL "virtual"

list_t *parseFile(char *filename);

// --------------------------------------------------------------- //

int main(int argc, char *argv[]) {
    char *filename;
    char *memoryType;
    int quantum;
    //  parse command line arguments
    for (int i = 1; i < argc; i++) {
        //  get arguments following flags
        if (argv[i][0] == DASH) {
            char flag = argv[i][1];
            switch (flag) {
            case FILE_FLAG:
                filename = (char *)safeMalloc(sizeof(char) * (strlen(argv[++i]) + 1));
                strcpy(filename, argv[i]);
                break;
            case MEM_FLAG:
                memoryType = (char *)safeMalloc(sizeof(char) * (strlen(argv[++i]) + 1));
                strcpy(memoryType, argv[i]);
                break;
            case QUANTUM_FLAG:
                quantum = atoi(argv[++i]);
                break;

            default:
                continue;
            }
        }
        else {
            printf("Incorrectly formatted command line arguments.\n");
            exit(EXIT_FAILURE);
        }
    }

    
    list_t *processList = parseFile(filename);


    //  task one, infinite memory
    if (strcmp(memoryType, INFINITE) == 0){
        runTaskOne(processList, quantum);
    }
    else if (strcmp(memoryType, FIRST_FIT) == 0){
        runTaskTwo(processList, quantum);
    }
    else if (strcmp(memoryType, PAGED) == 0){
        runTaskThree(processList, quantum);
    }
    else if (strcmp(memoryType, VIRTUAL) == 0){
        runTaskFour(processList, quantum);
    }



    //  free stored command line variables
    if (filename){
        free(filename);
    }
    if (memoryType){
        free(memoryType);
    }
    freeList(processList, &freeProcess);


    return 0;   
}
// --------------------------------------------------------------- //

//  parses a file and returns list of processes
list_t *parseFile(char *filename){
    FILE *fp = fopen(filename, "r");

    //  check if file exists/opens
    if(fp == NULL) {
        printf("File failed to open.");
        exit(EXIT_FAILURE);
    }


    //  read the file
    char line[BUFFER_SIZE];
    list_t *list = createList();
    while (fgets(line, BUFFER_SIZE, fp)){
        int arrivalTime = atoi(strtok(line, SPACE));
        char *processName = strtok(NULL, SPACE);
        int serviceTime = atoi(strtok(NULL, SPACE));
        short memoryRequirement = atoi(strtok(NULL, SPACE));
        process_t *process = createProcess(arrivalTime, processName, serviceTime, memoryRequirement);
        node_t *node = createNode(process);
        appendNode(node, list);
    }   


    fclose(fp);
    return list;
}