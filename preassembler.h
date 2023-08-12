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

#define FALSE 0
#define TRUE 1
/*macro for freeing all the allocated space in the macros (the pointers attrabiutes and the pointer itself)*/

enum Errors {Extratanatious_Text_After_Macro_Declaration = 0, Macro_Already_Exists, Macro_Name_Is_Saved_Word};

typedef struct macro{
    char* name;
    char* code;

    struct macro* next;
} macro;

int preassembler_invoke(char* filename);
int free_macros(macro* head);
int read_input_file(FILE** sourceFile, char* filename, char ending[3], char line[MAX_LINE_LENGTH], int* lineNum);
FILE* open_file(char* filename, char* ending, char* mode);
macro* add_macro_node(macro* head, char** name, int* errorStatus, int lineNum);
int error_handler(int errorCode, int lineNum);
int add_macro_codelines(FILE** asFile, char* code, char* line, char* originalLine, int* lineNum);
int write_line(FILE** amFile, macro* head, char* token, char originalLine[], char line[])