#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include "shared.h"

#define MAX_MACRO_NAME_LENGTH 30 // need to change that!
#define DELIMITERS " \t\n"

enum MacroErrors {Macro_Already_Exists = 0, Extratanatious_Text_After_Macro_Declaration, Macro_Name_Is_Saved_Word};

typedef struct macro{
    char name[MAX_MACRO_NAME_LENGTH];
    char* code;
    struct macro* next;
} macro;

int check_for_macro_erros(char** token, macro* head, int* status, int lineNum);
macro* add_macro_node(char* macroName, macro* head);
int free_macros(macro* head);
macro* find_macro(macro* head, char* token);
int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line, int* lineNum, int* status);
int write_line(FILE** amFile, macro* head, char* token, char* writeLine);
int invoke_preassembler(char* filename);
int error_handler(int errorCode, int lineNum);

#endif /*PREASSEMBLER_H*/