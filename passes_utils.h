#ifndef _UTILS_H
#define _UTILS_H

#include "shared.h"

#define MAX_PROG_MEMORY_SIZE 924

#define CHECK_ALLOCATION_ERROR(ptr) if(ptr == NULL) {printf("Assembler: encountered allocation error.\nTerminating program...\n"); exit(1);}
#define CHECK_FILE_DOESNT_EXIST(asPtr) if(asPtr == NULL) {printf("Assembler: couldn't open '.as' prompted file. (are you sure it has .as ending?) Moving to the next file...\n"); return ERROR;}
#define CHECK_OPENING_FALIURE(filePtr) if(filePtr == NULL) {printf("Assembler: couldn't open a file.\nTerminating program...\n"); exit(1);}
#define CHECK_BUFFER_OVERFLOW(ic, dc) if (ic + dc > MAX_PROG_MEMORY_SIZE-1) {printf("Assembler: program is exceeding the maximum available memory size.\nTerminating program...\n"); exit(1);}
#define CHECK_EMPTY_FILE(asPtr, status) if(ftell(asPtr) == 0) {printf("Assembler: prompted '.as' file is empty. Moving to the next file...\n"); status = ERROR;}
#define CHECK_NO_CODE_FILE(asPtr, amPtr, status) if(ftell(asPtr) != 0 && ftell(amPtr) == 0) {printf("Assembler: no code to analyze in prompted '.as' file. Moving to the next file...\n"); status = ERROR;}

#define LAST_CHARACTER(string) *(string+strlen(string)-1)
#define CHAR_TO_INT(c) (c - '0')
#define INT_TO_CHAR(i) (i + '0')

#define IS_WHITENOTE(c) ((c == ' ' || c == '\t'))
#define IS_NEWLINE(c) ((c == '\n'))

/**
 * This function removes a file with a given filename and ending.
 * 
 * @param filename Name of the file.
 * @param ending File ending (extension).
 * @return The success status of removing a file.
 */

int remove_file(char* filename, char* ending);

/**
 * This function opens a file using the provided filename, ending, and mode.
 *
 * @param filename Name of the file.
 * @param ending File ending (extension).
 * @param mode File opening mode.
 * @return Pointer to the opened file.
 */
FILE* open_file(char* filename, char* ending, char* mode);

/**
 * This function adds a string to the counterArray and increments the counter.
 *
 * @param counterArray Pointer to the counterArray.
 * @param counter Pointer to the current counter value.
 * @param toAdd String to add to the counterArray.
 * @return The success status of adding a string to the counterArray.
 */
int add_to_counterArray(char*** counterArray, int* counter, char* toAdd);

/*DEBUGGING FUNCTIONS*/

#endif
