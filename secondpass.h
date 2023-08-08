#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_LABEL_LENGTH 31
#define MAX_FILENAME_LENGTH 20

#define _ENTRY 3
#define _EXTERN 4

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