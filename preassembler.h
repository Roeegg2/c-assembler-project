/**
 * Created by Roee Toledano on 7/6/2023.
 * This is the header file for the preassembler code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELIMITERS " \t\n"
#define MAX_LINE_LENGTH 82

/*macro for freeing all the allocated space in the macros (the pointers attrabiutes and the pointer itself)*/
#define FREE_ALL(macros) int j; for (j = 0; j < macroIndex+1; j++){free(macros[j].name); free(macros[j].code);} free(macros);

/*macro for closing all the opened files*/
#define CLOSE_ALL(asFile, amFile) fclose(asFile); fclose(amFile);

typedef struct macro{
    char* name;
    char* code;

    struct macro* next;
} macro;