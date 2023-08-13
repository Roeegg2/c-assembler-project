/*manager function. its job is to read from the input file, and report any errors that arise.*/
#include "utils.h"
#include "shared.h"

int read_input_file(FILE** sourceFile, char* filename, char* ending, char* line, int* lineNum){
    if (*lineNum == 0)    
        *sourceFile = open_file(filename, ending, "r"); 

    if (*sourceFile == NULL)
        return -1; /* error_handler(1, *lineNum); */

    if (fgets(line, MAX_LINE_LENGTH, *sourceFile) != NULL){
        (*lineNum)++;
        return 1;
    }

    return 0;
}

FILE* open_file(char* filename, char* ending, char* mode){
    FILE* file;
    char foo[MAX_FILENAME_LENGTH];  /* add max for file extension as well */

    strcpy(foo, filename);
    strcat(foo, ending);
    file = fopen(foo, mode);  /* change that "a" to mode when done testing */

    return file;
}

int add_to_counterArray(char ***counterArray, int *counter, char *toAdd){
    (*counterArray) = (char **)realloc((*counterArray), sizeof(char *) * (*counter + 1));
    (*counterArray)[*counter] = (char *)malloc(sizeof(char) * MAX_LABEL_LENGTH);

    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}
