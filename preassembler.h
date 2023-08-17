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

extern int pa_status;

/**
 * Invokes the preassembler on a given assembly source file.
 *
 * @param filename The name of the assembly source file to preprocess.
 * @return Returns the status of the preassembler: TRUE if successful, ERROR if an error occurred.
 */
int invoke_preassembler(char* filename);

/**
 * Writes a line to the output assembly file, replacing macros if necessary.
 *
 * @param amFile Pointer to the output assembly file.
 * @param head Pointer to the linked list of macro definitions.
 * @param token The token to search for in the macro list.
 * @param writeLine The line to be written to the output file.
 * @return Returns TRUE if successful.
 */
int write_line(FILE** amFile, macro* head, char* token, char* writeLine);

/**
 * Reads lines from the assembly source file until a non-comment, non-empty line is found.
 *
 * @param asFile Pointer to the assembly source file.
 * @param originalLine Buffer to hold the original line read from the file.
 * @param line Buffer to hold the line without leading spaces or comments.
 * @param token Pointer to store the first token in the line.
 * @param lineNum Pointer to the current line number.
 * @return Returns TRUE if a valid line is found, or FALSE if end of file is reached.
 */
int func(FILE** asFile, char* originalLine, char* line, char** token, int* lineNum);

/**
 * Reads and processes the code of a macro from the assembly source file.
 *
 * @param asFile Pointer to the assembly source file.
 * @param head Pointer to the linked list of macro definitions.
 * @param originalLine Buffer to hold the original line read from the file.
 * @param line Buffer to hold the line without leading spaces or comments.
 * @param lineNum Pointer to the current line number.
 * @return Returns TRUE if macro code is successfully read and processed.
 */
int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line, int* lineNum);

/**
 * Frees memory allocated for the macro linked list.
 *
 * @param head Pointer to the head of the macro linked list.
 * @return Returns TRUE if successful.
 */
int free_macros(macro* head);

/**
 * Adds a new macro node to the macro linked list.
 *
 * @param macroName The name of the new macro.
 * @param head Pointer to the head of the macro linked list.
 * @return Returns a pointer to the new macro node.
 */
macro* add_macro_node(char* macroName, macro* head);

/**
 * Finds a macro node in the macro linked list by its name.
 *
 * @param head Pointer to the head of the macro linked list.
 * @param token The name of the macro to search for.
 * @return Returns a pointer to the macro node if found, or NULL if not found.
 */
macro* find_macro(macro* head, char* token);

/**
 * Checks for errors in macro declaration and name legality.
 *
 * @param token Pointer to the current token in the line.
 * @param head Pointer to the linked list of macro definitions.
 * @param lineNum The current line number.
 * @return Returns TRUE if no errors are found.
 */
int check_for_macro_erros(char** token, macro* head, int lineNum);

/**
 * Handles preassembler error codes and displays corresponding error messages.
 *
 * @param errorCode The error code to handle.
 * @param lineNum The line number where the error occurred.
 * @return Returns the pa_status after handling the error.
 */
int pa_error_handler(int errorCode, int lineNum);

/**
 * Checks whether a label or macro name is legal according to assembly language rules.
 *
 * @param name The label or macro name to check.
 * @param lineNum The current line number.
 * @param error_handler Function pointer to the error handler.
 * @return Returns TRUE if the name is legal, or an error code if not.
 */
int legal_label_macro_name(char* name, int lineNum, int (*error_handler)(int, int));

/**
 * Checks whether a line exceeds the maximum allowed length.
 *
 * @param line The line to check.
 * @param lineNum The current line number.
 * @return Returns TRUE if the line is within the allowed length, or FALSE if it's too long.
 */
int check_line_too_long(char* line, int lineNum);

#endif /*PREASSEMBLER_H*/
