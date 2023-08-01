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
    char name[MAX_LABEL_LENGTH];
    int line;
    int type; // : 2
    int address; 
} label;

typedef struct extentlabel{
    char name[MAX_LABEL_LENGTH];
    int type;
    int address;
    struct extentlabel* next;
} extentlabel;