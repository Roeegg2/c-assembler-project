#ifndef _SHARED_FP_PA_H
#define _SHARED_FP_PA_H

#include "shared.h"

/* change these later */
#define MAX_LINE_LENGTH 82 

enum FP_PA_SharedErrors {Illegal_Name_First_Char, Illegal_Name_Illegal_Chars, Illegal_Name_Saved_Word, Name_Too_Long};

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

#endif /* _SHARED_FP_PA_H */