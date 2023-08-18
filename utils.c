/*manager function. its job is to read from the input file, and report any errors that arise.*/
#include "utils.h"
#include "shared.h"

int read_input_file(FILE** sourceFile, char* filename, char* ending, char* line, int* lineNum){
    if (*lineNum == 0){
        *sourceFile = open_file(filename, ending, "r"); 
        CHECK_ENTERED_INVALID_FILE(*sourceFile);
    }

    if (fgets(line, MAX_LINE_LENGTH, *sourceFile) == NULL)
        return FALSE;
    
    (*lineNum)++;
    return TRUE;
}

FILE* open_file(char* filename, char* ending, char* mode){
    FILE* file;
    char foo[MAX_FILENAME_LENGTH+3];

    /*Get the full file name*/
    strcpy(foo, filename);
    strcat(foo, ending);

    /*open the file*/
    file = fopen(foo, mode);

    return file;
}

int add_to_counterArray(char*** counterArray, int* counter, char* toAdd){
    char* foo;

    /*Adding another word slot*/
    (*counterArray) = (char **)realloc((*counterArray), sizeof(char *) * (*counter + 1));
    CHECK_ALLOCATION_ERROR(*counterArray)

    /*Allocating for space for the new word*/
    (*counterArray)[*counter] = (char *)malloc(sizeof(char) * strlen(toAdd));
    CHECK_ALLOCATION_ERROR(*counterArray)
    
    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}

int free_extent(extentlabel* head){
    extentlabel* foo;

    while (head != NULL){
        free(head->address.addr); /*Free the addr field*/
        foo = head; /*Saving head in temp to be able to free head node*/
        head = head->next; 
        free(foo);
    }

    return TRUE;
}

int free_counter_array(char*** counterImage, int counter){
    int i;

    for (i = 0; i < counter; i++)
        if ((*counterImage)[i] != NULL) /*Freeing each char in word allocated*/
            free((*counterImage)[i]);
    
    free((*counterImage)); /*freeing the main pointer (not pointer to pointer)*/
    
    return TRUE;
}
