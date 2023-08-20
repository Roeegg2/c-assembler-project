#include "passes_utils.h"

#define MAX_FILENAME_LENGTH 20

FILE* open_file(char* filename, char* ending, char* mode){
    FILE* file;
    char foo[MAX_FILENAME_LENGTH+4];

    /*Get the full file name*/
    strcpy(foo, filename);
    strcat(foo, ending);

    /*open the file*/
    file = fopen(foo, mode);

    return file;
}

int remove_file(char* filename, char* ending){
    char foo[MAX_FILENAME_LENGTH+4];

    strcpy(foo, filename);
    strcat(foo, ending);
    remove(foo);

    return TRUE;
}

int add_to_counterArray(char*** counterArray, int* counter, char* toAdd){
    /*Adding another word slot*/
    (*counterArray) = (char **)realloc((*counterArray), sizeof(char *) * (*counter + 1));
    CHECK_ALLOCATION_ERROR(*counterArray)

    /*Allocating for space for the new word*/
    (*counterArray)[*counter] = (char *)malloc(sizeof(char) * MAX_LABEL_MACRO_LENGTH);
    CHECK_ALLOCATION_ERROR(*counterArray)
    
    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}
