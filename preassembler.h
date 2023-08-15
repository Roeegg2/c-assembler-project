#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include "shared.h"

/* need to change that! */
#define MAX_MACRO_NAME_LENGTH 30 
#define DELIMITERS " \t\n"

enum MacroErrors {Macro_Already_Exists = 3, Extratanatious_Text_After_Macro_Declaration, Line_Too_Long};

typedef struct macro{
    char name[MAX_MACRO_NAME_LENGTH];
    char* code;
    struct macro* next;
} macro;

int check_for_macro_erros(char** token, macro* head, int lineNum);
macro* add_macro_node(char* macroName, macro* head);
macro* find_macro(macro* head, char* token);
int free_macros(macro* head);
int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line, int* lineNum);
int write_line(FILE** amFile, macro* head, char* token, char* writeLine);
int invoke_preassembler(char* filename);
int pa_error_handler(int errorCode, int lineNum);
/* int check_line_too_long(FILE** sourceFile, int* status, int lineNum); */

#endif /*PREASSEMBLER_H*/