#ifndef _SHARED_FP_SP_H
#define _SHARED_FP_SP_H

#include "shared.h"

#define _DATA 16
#define _STRING 17
#define _ENTRY 18
#define _EXTERN 19

#define ABSOLUTE "00"
#define RELOCATABLE "10"

/*12 - which is the size of word +1 terminator*/
#define SIZE_OF_WORD 13

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

#endif /* _SHARED_FP_SP_H */