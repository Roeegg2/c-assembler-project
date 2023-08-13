#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// change these later
#define MAX_LINE_LENGTH 100 
#define MAX_LABEL_LENGTH 31
#define MAX_FILENAME_LENGTH 20

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define _DATA 1
#define _STRING 2
#define _ENTRY 3
#define _EXTERN 4
#define _INSTRUCTION 6
#define _NONE 5

typedef struct label{
    char labelName[MAX_LABEL_LENGTH];
    int line;
    int type; // : 2
    int address; 
} label;

typedef struct extentlabel{
    char labelName[MAX_LABEL_LENGTH];
    struct{
        int* addr;
        int count;
    } address;
    int type;
    struct extentlabel* next;
} extentlabel;