#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include "shared.h"

/* check all of these and make sure they are right! */
#define SIZE_OF_WORD 12

#define MAX_DATA_LENGTH 10 
#define MAX_OPERANDS 2

#define PARAM_DELIMITERS " \t\n,"

#define ABSOLUTE "00"
#define RELOCATABLE "10"
#define EXTERNAL "01"

#define _WHITENOTE 1
#define _COMMA 2
#define _CHAR 3

#define _SOURCE 0
#define _DEST 1

#define BOTH_ARE_NOT_NULL(sourceStatus, destStatus) (sourceStatus == 1 && destStatus == 1)
#define BOTH_ARE_REGISTER(op) (op->destOperand->addrMode == Register && op->sourceOperand->addrMode == Register)

#define CHAR_TO_INT(c) (c - '0')

enum Opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum AddressingModes {No_Operand = 0, Immediate = 1, Direct = 3, Register = 5};
enum FirstpassErrors {Unknown_Command = 3, Comma_Error, Extra_Comma, Double_Comma, Missing_Comma, 
    Parameter_Not_Whole_Number, Parameter_Out_Of_Bounds, Invalid_Source_Sequence, Invalid_Dest_Sequence, 
    Illegal_String_Declaration, Label_Already_Defined, Extent_Label_Already_Defined_Differently, 
    Label_Name_Too_Long, Undefined_Register, Blank_Label_Declaration, Missing_Operand, 
    Too_Many_Operands, No_Lables_Extent_Marking, Illegal_String_Char, No_Params_DataString_Instruction};
enum Warnings {Label_Points_At_ExternEntry = 0, Extent_Label_Already_Defined_Similarly};

typedef struct operand{
    int addrMode;
    union {
        char labelName[MAX_LABEL_LENGTH];
        int regNum;
        int numericVal;
    } val; 
} operand;

typedef struct operation{
    int opcode;
    /* char paramSequence[2] */
    operand* sourceOperand;
    operand* destOperand;
} operation;

/**
 * Converts a decimal number to a binary string representation.
 *
 * @param binary - The output binary string.
 * @param number - The decimal number to be converted.
 * @param size   - The size of the binary string buffer.
 * @return 0 on success.
 */
int convert_to_binary(char binary[], int number, int size);

/**
 * Flips the bits of a binary string representing a negative number.
 *
 * @param binary - The binary string to be flipped.
 * @return TRUE on success.
 */
int flip_negative(char binary[]);

/**
 * Adds one to a binary string representing a positive number.
 *
 * @param binary - The binary string to which one will be added.
 * @return TRUE on success.
 */
int add_one(char binary[]);

/**
 * Adds a label to the label table.
 *
 * @param labelTable  - Pointer to the label table.
 * @param labelName   - Name of the label.
 * @param labelCount  - Pointer to the label count.
 * @param counterValue - Current counter value.
 * @param isData       - Flag indicating if the label corresponds to data.
 * @param lineNum      - Line number of the label declaration.
 * @return TRUE on success.
 */
int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int isData, int lineNum);

/**
 * Adds the Absolute Relocatable External (ARE) code to a binary string.
 *
 * @param binary - The binary string to which the ARE code will be added.
 * @param are    - The ARE code to be added.
 * @return 0.
 */
int add_are(char *binary, char *are);

/**
 * Adds the first operand word of an operation to the instruction counter image.
 *
 * @param icImage - Pointer to the instruction counter image.
 * @param op      - Pointer to the operation.
 * @param ic      - Pointer to the instruction counter.
 * @return 0.
 */
int add_first_op_word(char ***icImage, operation *op, int *ic);

/**
 * Checks if a given string corresponds to an assembly operation.
 *
 * @param opname - The string to be checked.
 * @return The operation code if it's a valid operation, or -1 if not.
 */
int is_operation(char *opname);

/**
 * Analyzes a string literal and stores its characters as integer parameters.
 *
 * @param params      - Pointer to the parameter array.
 * @param stringLine  - The string literal to be analyzed.
 * @param lineNum     - Line number of the string declaration.
 * @return The number of characters in the string + 1 (for null terminator) on success.
 */
int analyze_string(int **params, char *stringLine, int lineNum);

/**
 * Analyzes data parameters and stores them as integer parameters.
 *
 * @param token   - Pointer to the token string.
 * @param params  - Pointer to the parameter array.
 * @param lineNum - Line number of the data declaration.
 * @return TRUE on success.
 */
int analyze_data(char **token, int **params, int lineNum);

/**
 * Adds words to the data counter image representing a data string.
 *
 * @param dcImage   - Pointer to the data counter image.
 * @param params    - Pointer to the parameter array.
 * @param dc        - Pointer to the data counter.
 * @param paramCnt  - Number of parameters in the array.
 * @param lineNum   - Line number of the data declaration.
 * @return TRUE on success.
 */
int add_datastring_word(char ***dcImage, int *params, int *dc, int paramCnt, int lineNum);

/**
 * Finds an extent label by name in the linked list.
 *
 * @param head      - Pointer to the head of the linked list.
 * @param labelName - The name of the label to find.
 * @return Pointer to the found label or NULL if not found.
 */
extentlabel *find_extent_label(extentlabel *head, char *labelName);

/**
 * Adds an extent label to the linked list.
 *
 * @param head      - Pointer to the head of the linked list.
 * @param token     - Pointer to the token string containing the label name.
 * @param type      - The type of the label.
 * @param lineNum   - Line number of the label declaration.
 * @return TRUE on success.
 */
int add_extent_label(extentlabel **head, char **token, int type, int lineNum);

/**
 * Checks if the given token is a label declaration.
 *
 * @param token     - Pointer to the token string.
 * @param labelName - The label name extracted from the token.
 * @param lineNum   - Line number of the token.
 * @return FALSE if not a label declaration, otherwise, the result of legal_label_macro_name function.
 */
int is_label(char **token, char *labelName, int lineNum);

/**
 * Finds a label in the label table by its name.
 *
 * @param labelTable  - Pointer to the label table.
 * @param labelName   - The name of the label to find.
 * @param labelCount  - Number of labels in the table.
 * @return Pointer to the found label or NULL if not found.
 */
label *find_label(label *labelTable, char *labelName, int labelCount);

/**
 * Adds operand words to the instruction counter image.
 *
 * @param icImage     - Pointer to the instruction counter image.
 * @param labelTable  - Pointer to the label table.
 * @param op          - Pointer to the operation.
 * @param ic          - Pointer to the instruction counter.
 * @param labelCount  - Number of labels in the table.
 * @return TRUE on success.
 */
int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount);

/**
 * Gets the value of an operand from a token and stores it in the operand structure.
 *
 * @param op       - Pointer to the operand structure.
 * @param token    - Pointer to the token string.
 * @param lineNum  - Line number of the token.
 * @return TRUE on success.
 */
int get_operand_value(operand *op, char *token, int lineNum);

/**
 * Gets the value of an operand depending on its address mode.
 *
 * @param labelTable  - Pointer to the label table.
 * @param operandd    - Pointer to the operand structure.
 * @param val         - Pointer to the integer value to be retrieved.
 * @param labelCount  - Number of labels in the table.
 * @return TRUE on success.
 */
int get_type_val(label **labelTable, operand *operandd, int *val, int labelCount);

/**
 * Generates a binary representation of a pair of register values.
 *
 * @param binary     - The output binary string.
 * @param sourceVal  - Value of the source register.
 * @param destVal    - Value of the destination register.
 * @return TRUE on success.
 */
int get_register_word(char *binary, int sourceVal, int destVal);

/**
 * Generates a binary representation of an operand value in isolated mode.
 *
 * @param operandd  - Pointer to the operand structure.
 * @param binary    - The output binary string.
 * @param val       - Operand value.
 * @param fp_status - Status indicating operand validity.
 * @return TRUE if operand is valid, otherwise FALSE.
 */
int get_isolated_word(operand *operandd, char *binary, int val, int fp_status);

/**
 * Sets the source and destination operands of an operation.
 *
 * @param op          - Pointer to the operation structure.
 * @param operand1    - Pointer to the first operand structure.
 * @param operand2    - Pointer to the second operand structure.
 * @return TRUE on success.
 */
int set_operands(operation *op, operand *operand1, operand *operand2);

/**
 * Gets the count of parameters separated by commas in a line.
 *
 * @param line     - The line containing the parameters.
 * @param lineNum  - Line number of the line.
 * @return The count of parameters.
 */
int get_comma_param_cnt(char *line, int lineNum);

/**
 * Checks if a given token is an extent instruction (entry or extern).
 *
 * @param token - The token to be checked.
 * @return The type of extent instruction if it matches, otherwise FALSE.
 */
int is_extent_instruction(char *token);

/**
 * Checks if a given token is a data or string instruction.
 *
 * @param token - The token to be checked.
 * @return The type of data or string instruction if it matches, otherwise FALSE.
 */
int is_datastring_instruction(char *token);

/**
 * Calls the datastring analyzer function based on the commandCode and handles data or string instructions.
 *
 * @param lineToken       - Pointer to the tokenized line.
 * @param params          - Pointer to the array of parameter values.
 * @param orgLineToken    - Original line token string.
 * @param lineNum         - Line number.
 * @param commandCode     - Command code indicating whether it's .data or .string.
 * @return The number of parameters analyzed.
 */
int call_datastring_analyzer(char **lineToken, int **params, char *orgLineToken, int lineNum, int commandCode);

/**
 * Retrieves a pointer to the first parameter in a line after a specific character.
 *
 * @param orgLineToken  - Original line token string.
 * @param toFind        - The character to find after which the parameter pointer is needed.
 * @return Pointer to the first parameter after the character, or NULL if not found.
 */
char *get_param_pointer(char *orgLineToken, char toFind);

/**
 * Sets the sequence array for destination operands in operations.
 *
 * @param sequenceArray - The destination operand sequence array to be set.
 * @return TRUE on success.
 */
int set_sequence_array_dest(char sequenceArray[16][4]);

/**
 * Sets the sequence array for source operands in operations.
 *
 * @param sequenceArray - The source operand sequence array to be set.
 * @return TRUE on success.
 */
int set_sequence_array_source(char sequenceArray[16][4]);

/**
 * Handles extent label declaration.
 *
 * @param head         - Pointer to the extent label list head.
 * @param token        - Pointer to the current token.
 * @param originalLine - Original line string.
 * @param commandCode  - Command code (_EXTERN or _ENTRY).
 * @param lineNum      - Line number.
 * @return TRUE on success.
 */
int extent_handler(extentlabel **head, char **token, char *originalLine, int commandCode, int lineNum);

/**
 * Handles data or string instructions during the first pass.
 *
 * @param dcImage       - Pointer to the data counter image.
 * @param token         - Pointer to the current token.
 * @param originalLine  - Original line string.
 * @param dc            - Pointer to the data counter.
 * @param commandCode   - Command code indicating .data or .string.
 * @param lineNum       - Line number.
 * @return TRUE on success.
 */
int datastring_handler(char ***dcImage, char **token, char *originalLine, int *dc, int commandCode, int lineNum);

/**
 * Handles operations during the first pass.
 *
 * @param icImage          - Pointer to the instruction counter image.
 * @param labelTable       - Pointer to the label table.
 * @param token            - Pointer to the current token.
 * @param originalLine     - Original line string.
 * @param ic               - Pointer to the instruction counter.
 * @param labelCount       - Pointer to the label count.
 * @param commandCode      - Command code indicating the operation.
 * @param lineNum          - Line number.
 * @param sourceSequenceArray - Sequence array for source operands.
 * @param destSequenceArray   - Sequence array for destination operands.
 * @return TRUE on success.
 */
int operation_handler(char ***icImage, label **labelTable, char **token, char *originalLine, int *ic, int *labelCount, int commandCode, int lineNum, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);

/**
 * Checks if the operand sequence is valid for an operation.
 *
 * @param sequenceArr - Operand sequence array (source or destination).
 * @param operandd    - Pointer to the operand structure.
 * @param opcode      - Operation opcode.
 * @param lineNum     - Line number.
 * @param sourceOrDest - Flag indicating whether the sequence is for source or destination.
 * @return TRUE if the sequence is valid, otherwise an error code.
 */
int check_param_sequence(char sequenceArr[16][4], operand *operandd, int opcode, int lineNum, int sourceOrDest);

/**
 * Handles warnings during the first pass.
 *
 * @param warningCode - Warning code indicating the type of warning.
 * @param lineNum     - Line number.
 * @return TRUE on success.
 */
int fp_warning_handler(int warningCode, int lineNum);

/**
 * Handles errors during the first pass.
 *
 * @param errorCode  - Error code indicating the type of error.
 * @param lineNum    - Line number.
 * @return An error code (ERROR) indicating an error occurred.
 */
int fp_error_handler(int errorCode, int lineNum);

/**
 * Performs the first pass of the assembly process for the given assembly code file.
 *
 * @param dcImage      Double pointer to the data code (dc) image array.
 * @param icImage      Double pointer to the instruction code (ic) image array.
 * @param labelTable   Double pointer to the label table array.
 * @param head         Double pointer to the extent label linked list.
 * @param filename     Pointer to the filename of the assembly code file.
 * @param dc           Pointer to the data counter value.
 * @param ic           Pointer to the instruction counter value.
 * @param labelCount   Pointer to the label count value.
 *
 * @return Returns the status of the assembly process.
 */
int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount);


#endif /* FIRSTPASS_H */
