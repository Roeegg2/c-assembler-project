#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_ALLOCATION_ERROR(ptr) if(ptr == NULL) {printf("Allocation Error (until i add a function manager this message will also appear when opening file failed). Terminating program\n"); exit(1);}

FILE* open_file(char* filename, char* ending, char* mode);
int read_input_file(FILE** sourceFile, char* filename, char ending[], char line[], int* lineNum);
int add_to_counterArray(char ***counterArray, int *counter, char *toAdd);

/*DEBUGGING FUNCTIONS*/
