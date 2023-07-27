#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Created by Roee Toledano on 7/6/2023.
 * This is the header file for the preassembler code.
 */
#define DELIMITERS " \t\n"
#define MAX_LINE_LENGTH 82
#define MAX_FILENAME_LENGTH 30
/*macro for freeing all the allocated space in the macros (the pointers attrabiutes and the pointer itself)*/
typedef struct macro{
    char* name;
    char* code;

    struct macro* next;
} macro;

int read_input_file(FILE** sourceFile, char* filename, char ending[3], char line[MAX_LINE_LENGTH], int* lineNum);