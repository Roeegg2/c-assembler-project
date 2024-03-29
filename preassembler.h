#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H

#include "shared_fp_pa.h"

#define DELIMITERS " \t\n"

enum MacroErrors {Macro_Already_Exists = 4, Extratanatious_Text_After_Macro_Declaration, Line_Too_Long};

typedef struct macro{
    char name[MAX_LABEL_MACRO_LENGTH];
    char* code;
    struct macro* next;
} macro;

/**
 * This function checks if a word is a mcro or endmcro word.
 *
 * @param token Word to check.
 * @return True if the word is a mcro or endmcro word, false otherwise.
 */
int is_mcro_or_endmcro_word(char* token);

/**
 * This function checks if a line is too long and handles related errors.
 *
 * @param line Line to check.
 * @param asFile Pointer to the .as file
 * @return The success status of checking the line length.
 */

int check_line_too_long(FILE** asFile, char* line);

/**
 * This function starts the preassembler process, which processes macro declarations, expands macros,
 * and writes the output .am file.
 *
 * @param filename Name of the source file.
 * @return The success status of invoking the preassembler.
 */
int invoke_preassembler(char* filename);

/**
 * This function writes a line to the .am file, considering macro expansion if the line contains a macro call.
 *
 * @param amFile Pointer to the .am file.
 * @param head Pointer to the macro linked list.
 * @param token First token of the line.
 * @param writeLine Line to write.
 * @return The success status of writing the line.
 */
int write_line(FILE** amFile, macro* head, char** token, char* writeLine);

/**
 * This function reads and skips lines until a non-comment, non-blank line is encountered.
 *
 * @param asFile Pointer to the .as file.
 * @param originalLine Original line read from the file.
 * @param line Processed line to tokenize.
 * @param token Pointer to the tokenized line.
 * @return The success status of reading lines.
 */
int get_code_valid_line(FILE** asFile, char* originalLine, char* line, char** token);

/**
 * This function gets the macro code from the source file and stores it in the macro node.
 *
 * @param asFile Pointer to the .as file.
 * @param head Pointer to the macro linked list.
 * @param originalLine Original line read from the file.
 * @param line Processed line to tokenize.
 * @return The success status of getting macro code.
 */
int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line);

/**
 * This function frees the memory allocated for the macro linked list.
 *
 * @param head Pointer to the macro linked list.
 * @return The success status of freeing the macros.
 */
int free_macros(macro* head);

/**
 * This function adds a new macro node to the macro linked list.
 *
 * @param macroName Name of the macro.
 * @param head Pointer to the macro linked list.
 * @return Pointer to the new macro node.
 */
macro* add_macro_node(char* macroName, macro* head);

/**
 * This function finds a macro node with a given name in the macro linked list.
 *
 * @param head Pointer to the macro linked list.
 * @param token Macro name to find.
 * @return Pointer to the found macro node or NULL if not found.
 */
macro* find_macro(macro* head, char* token);

/**
 * This function checks for errors related to macro declarations and their names.
 *
 * @param token Pointer to the current token.
 * @param head Pointer to the macro linked list.
 * @return The success status of error checking.
 */
int check_for_macro_erros(char** token, macro* head);

/**
 * This function handles and reports errors specific to the preassembler, setting the pa_status flag accordingly.
 *
 * @param errorCode Error code indicating the type of error.
 * @return The status indicating whether an error occurred.
 */
int pa_error_handler(int errorCode);

#endif /*PREASSEMBLER_H*/
