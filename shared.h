#ifndef _SHARED_H
#define _SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* change these later */
#define MAX_LINE_LENGTH 82
#define MAX_LABEL_LENGTH 31
#define MAX_FILENAME_LENGTH 20

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define _DATA 16
#define _STRING 17
#define _ENTRY 18
#define _EXTERN 19

enum SharedErrors {Illegal_Name_First_Char, Illegal_Name_Illegal_Chars, Illegal_Name_Saved_Word};

typedef struct label{
    char labelName[MAX_LABEL_LENGTH];
    int line;
    int address;
    int isInstruction;
} label;

typedef struct extentlabel{
    char labelName[MAX_LABEL_LENGTH];
    struct{
        int* addr;
        int count;
    } address;
    int type;
    int line;
    struct extentlabel* next;
} extentlabel;

label* find_label(label* labelTable, char* labelName, int labelCount);
extentlabel* find_extent_label(extentlabel* head, char* labelName);
int convert_to_binary(char binary[], int number, int size);

int legal_label_macro_name(char* name, int(*error_handler)(int));

int is_operation(char* opname);
int is_datastring_instruction(char* token);
int is_extent_instruction(char* token);
#endif
