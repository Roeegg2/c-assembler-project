#ifndef _UTILS_H
#define _UTILS_H

#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROG_MEMORY_SIZE 924

#define CHECK_ALLOCATION_ERROR(ptr) if(ptr == NULL) {printf("Program encountered allocation error.\nTerminating program...\n"); exit(1);}
#define CHECK_ENTERED_INVALID_FILE(ptr) if(ptr == NULL) {printf("Couldn't open a prompted file. (are you sure it has .as ending?) Moving to the next file...\n"); return FALSE;}
#define CHECK_FILE_STATUS(ptr) if(ptr == NULL) {printf("Program couldn't open a file.\nTerminating program...\n"); exit(1);}
#define CHECK_EMPTY_FILE(filePtr, lineNum, status) if (lineNum == 0 && filePtr != NULL) {printf("Entered empty file.Ignoring current file and moving to the next file...\n"); status = FALSE;}
#define CHECK_BUFFER_OVERFLOW(ic, dc) if (ic + dc > MAX_PROG_MEMORY_SIZE) {printf("Program is exceeding the maximum available memory size.\nTerminating program...\n"); exit(1);}

#define LAST_CHARACTER(string) *(string+strlen(string)-1)

/**
 * Reads a line from an input source file.
 *
 * @param sourceFile Pointer to the source file.
 * @param filename The name of the assembly source file.
 * @param ending The file extension to append to the filename.
 * @param line Buffer to store the read line.
 * @param lineNum Pointer to the current line number.
 * @return Returns TRUE if a line is successfully read, or FALSE if the end of the file is reached.
 */
int read_input_file(FILE** sourceFile, char* filename, char* ending, char* line, int* lineNum);

/**
 * Opens a file with the given filename, file extension, and mode.
 *
 * @param filename The name of the file.
 * @param ending The file extension to append to the filename.
 * @param mode The mode to open the file in (e.g., "r" for read, "w" for write).
 * @return Returns a pointer to the opened file.
 */
FILE* open_file(char* filename, char* ending, char* mode);

/**
 * Adds a string to the counter array.
 *
 * @param counterArray Pointer to the counter array.
 * @param counter Pointer to the counter value.
 * @param toAdd The string to add to the counter array.
 * @return Returns TRUE if the string is successfully added.
 */
int add_to_counterArray(char*** counterArray, int* counter, char* toAdd);

/**
 * Frees memory allocated for an extent label linked list.
 *
 * @param head Pointer to the head of the extent label linked list.
 * @return Returns TRUE if memory is successfully freed.
 */
int free_extent(extentlabel* head);

/**
 * Frees memory allocated for a counter array.
 *
 * @param counterImage Pointer to the counter array.
 * @param counter The counter value.
 * @return Returns TRUE if memory is successfully freed.
 */
int free_counter_array(char*** counterImage, int counter);

#endif /* _  UTILS_H */
