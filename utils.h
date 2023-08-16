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
#define CHECK_BUFFER_OVERFLOW(ic, dc) if (ic + dc > MAX_PROG_MEMORY_SIZE) {printf("Program is exceeding the maximum available memory size.\nTerminating program...\n"); exit(1);}

#define LAST_CHARACTER(string) *(string+strlen(string)-1)

FILE* open_file(char* filename, char* ending, char* mode);
int read_input_file(FILE** sourceFile, char* filename, char ending[], char line[], int* lineNum);
int add_to_counterArray(char ***counterArray, int *counter, char *toAdd);
int free_extent(extentlabel* head);
int free_counter_array(char*** counterImage, int counter);
/*DEBUGGING FUNCTIONS*/

#endif
