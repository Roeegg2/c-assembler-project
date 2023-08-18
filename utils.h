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
#define CHECK_BUFFER_OVERFLOW(ic, dc) if (ic + dc > MAX_PROG_MEMORY_SIZE-1) {printf("Program is exceeding the maximum available memory size.\nTerminating program...\n"); exit(1);}

#define LAST_CHARACTER(string) *(string+strlen(string)-1)

/**
 * @brief Reads a line from the input source file.
 *
 * This function reads a line from the source file and increments the line number.
 *
 * @param sourceFile Pointer to the source file.
 * @param filename Name of the source file.
 * @param ending File ending (extension).
 * @param line Pointer to store the read line.
 * @param lineNum Pointer to the current line number.
 * @return The success status of reading a line.
 */
int read_input_file(FILE** sourceFile, char* filename, char* ending, char* line, int* lineNum);

/**
 * @brief Opens a file with a given filename and mode.
 *
 * This function opens a file using the provided filename, ending, and mode.
 *
 * @param filename Name of the file.
 * @param ending File ending (extension).
 * @param mode File opening mode.
 * @return Pointer to the opened file.
 */
FILE* open_file(char* filename, char* ending, char* mode);

/**
 * @brief Adds a string to the counterArray.
 *
 * This function adds a string to the counterArray and increments the counter.
 *
 * @param counterArray Pointer to the counterArray.
 * @param counter Pointer to the current counter value.
 * @param toAdd String to add to the counterArray.
 * @return The success status of adding a string to the counterArray.
 */
int add_to_counterArray(char*** counterArray, int* counter, char* toAdd);

/**
 * @brief Frees memory occupied by the extent label linked list.
 *
 * This function frees memory occupied by the extent label linked list.
 *
 * @param head Pointer to the head of the extent label linked list.
 * @return The success status of freeing the extent label linked list.
 */
int free_extent(extentlabel* head);

/**
 * @brief Frees memory occupied by the counterArray.
 *
 * This function frees memory occupied by the counterArray.
 *
 * @param counterImage Pointer to the counterArray.
 * @param counter Number of elements in the counterArray.
 * @return The success status of freeing the counterArray.
 */
int free_counter_array(char*** counterImage, int counter);

/*DEBUGGING FUNCTIONS*/

#endif
