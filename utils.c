/*manager function. its job is to read from the input file, and report any errors that arise.*/
#include "utils.h"
#include "shared.h"

int read_input_file(FILE** sourceFile, char* filename, char* ending, char* line, int* lineNum){
    memset(line, '\0', MAX_LINE_LENGTH);
    if (*lineNum == 0){
        *sourceFile = open_file(filename, ending, "r"); 
        return FALSE;
    }
    

    if (fgets(line, MAX_LINE_LENGTH, *sourceFile) == NULL)
        return FALSE;
       
    (*lineNum)++;
    return TRUE;
}

FILE* open_file(char* filename, char* ending, char* mode){
    FILE* file;
    char fullFileName[MAX_FILENAME_LENGTH+3];  /* add max for file extension as well */

    strcpy(fullFileName, filename);
    strcat(fullFileName, ending);
    
    file = fopen(fullFileName, mode);  /* change that "a" to mode when done testing */

    return file;
}

int remove_file(char* filename, char* ending){
    char fullFileName[MAX_FILENAME_LENGTH+3];  /* add max for file extension as well */

    strcpy(fullFileName, filename);
    strcat(fullFileName, ending);
    
    remove(fullFileName);
    
    return TRUE;
}

int add_to_counterArray(char ***counterArray, int *counter, char *toAdd){
    (*counterArray) = (char **)realloc((*counterArray), sizeof(char *) * (*counter + 1));
    CHECK_ALLOCATION_ERROR(*counterArray)
    
    (*counterArray)[*counter] = (char *)malloc(sizeof(char) * MAX_LABEL_LENGTH);
    CHECK_ALLOCATION_ERROR(*counterArray)
    
    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}

int free_extent(extentlabel* head){
    extentlabel* foo;

    while (head != NULL){
        free(head->address.addr);
        foo = head;
        head = head->next;
        free(foo);
    }

    return TRUE;
}

int free_counter_array(char*** counterImage, int counter){
    int i;

    for (i = 0; i < counter; i++)
        if ((*counterImage)[i] != NULL)
            free((*counterImage)[i]);
    
    free((*counterImage));
    
    return TRUE;
}
