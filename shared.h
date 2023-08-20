#ifndef _SHARED_H
#define _SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* change these later */
#define MAX_LINE_LENGTH 82 
#define MAX_LABEL_MACRO_LENGTH 31
#define MAX_FILENAME_LENGTH 20

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define _DATA 16
#define _STRING 17
#define _ENTRY 18
#define _EXTERN 19

enum SharedErrors {Illegal_Name_First_Char, Illegal_Name_Illegal_Chars, Illegal_Name_Saved_Word, Name_Too_Long};

typedef struct label{
    char labelName[MAX_LABEL_MACRO_LENGTH];
    int line;
    int address;
    int isInstruction;
} label;

typedef struct extentlabel{
    char labelName[MAX_LABEL_MACRO_LENGTH];
    struct{
        int* addr;
        int count;
    } address;
    int type;
    int line;
    struct extentlabel* next;
} extentlabel;


typedef struct maplabel{
    int icAddress;
    int lineNum;
    struct maplabel* next;
} maplabel;

/**
 * This function searches the label table for a label with a given name and returns its entry.
 *
 * @param labelTable Pointer to the label table array.
 * @param labelName The name of the label to be found.
 * @param labelCount The number of labels in the table.
 * @return The pointer to the label entry, or NULL if not found.
 */
label* find_label(label *labelTable, char *labelName, int labelCount);


/**
 * This function searches the extent label list for a label with a given name and returns its node.
 *
 * @param head Pointer to the head of the extent label list.
 * @param labelName The name of the label to be found.
 * @return The pointer to the extent label node, or NULL if not found.
 */
extentlabel* find_extent_label(extentlabel* head, char* labelName);

/**
 * This function converts an integer value to its binary representation of a specified size.
 * It handles both positive and negative numbers and performs 2's complement conversion for negative numbers.
 *
 * @param binary Pointer to the output buffer for the binary representation.
 * @param number The integer number to be converted.
 * @param size The size of the binary representation.
 * @return The success status of the conversion.
 */
int convert_to_binary(char binary[], int number, int size);

/**
 * This function checks if a given label or macro name is legal and follows naming conventions.
 *
 * @param name Label or macro name to check.
 * @param error_handler Function pointer to the error handler.
 * @return The success status of checking the label/macro name.
 */
int legal_label_macro_name(char* name, int(*error_handler)(int));

/**
 * This function maps an operation name to its corresponding opcode value.
 *
 * @param opname The operation name.
 * @return The opcode value for the operation, or -1 if not found.
 */
int is_operation(char *opname);

/**
 * This function checks if a token corresponds to a ".data" or ".string" instruction.
 *
 * @param token Pointer to the token to be checked.
 * @return The instruction type (DATADATA if ".data", DATASTRING if ".string", or FALSE if neither).
 */
int is_datastring_instruction(char* token);

/**
 * This function checks if a token corresponds to a ".entry" or ".extern" instruction.
 *
 * @param token Pointer to the token to be checked.
 * @return The instruction type (EXTENTRY if ".entry", EXTEXTERN if ".extern", or FALSE if neither).
 */
int is_extent_instruction(char* token);

#endif /*_SHARED_H*/