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
enum FirstpassErrors {Unknown_Command = 3, Extra_Comma, Double_Comma, Missing_Comma, 
    Parameter_Not_Whole_Number, Parameter_Out_Of_Bounds, Invalid_Source_Sequence, Invalid_Dest_Sequence, 
    Illegal_String_Declaration, Label_Already_Defined, Extent_Label_Already_Defined_Differently, 
    Label_Name_Too_Long, Undefined_Register, Blank_Label_Declaration, Missing_Dest_Operand, 
    Missing_Source_Operand, Too_Many_Operands, Blank_Extent_Marking, Illegal_String_Char, Blank_DataString_Instruction};
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
 * @brief Processes an assembly file, generates intermediate code, and builds a label table.
 *
 * This function reads an assembly file, processes each line, and generates intermediate code.
 * It also maintains a label table and handles label declarations, instructions, and operations.
 *
 * @param dcImage Pointer to the data image array.
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Pointer to the label table array.
 * @param head Pointer to the extent label list.
 * @param filename Name of the assembly file to be processed.
 * @param dc Pointer to the data counter.
 * @param ic Pointer to the instruction counter.
 * @param labelCount Pointer to the label count.
 * @param sourceSequenceArray Array of source operand sequence patterns.
 * @param destSequenceArray Array of destination operand sequence patterns.
 * @return The status of the function execution.
 */
int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);

/**
 * @brief Checks if an operand's addressing mode is valid for a given operation.
 *
 * This function validates whether the addressing mode of an operand is valid
 * for a specific operation by comparing it with the allowed addressing modes
 * defined in the sequence array.
 *
 * @param sequenceArr Array of operand sequence patterns for different operations.
 * @param operandd Pointer to the operand to be validated.
 * @param opcode Opcode of the operation being checked.
 * @param sourceOrDest Flag indicating whether the operand is a source or destination operand.
 * @return The validation status of the operand's addressing mode for the operation.
 */
int check_param_sequence(char sequenceArr[16][4], operand* operandd, int opcode, int sourceOrDest);

/**
 * @brief Converts an integer to its binary representation.
 *
 * This function converts an integer value to its binary representation of a specified size.
 * It handles both positive and negative numbers and performs 2's complement conversion for negative numbers.
 *
 * @param binary Pointer to the output buffer for the binary representation.
 * @param number The integer number to be converted.
 * @param size The size of the binary representation.
 * @return The success status of the conversion.
 */
int convert_to_binary(char binary[], int number, int size);

/**
 * @brief Flips the bits of a binary string to perform 1's complement.
 *
 * This function flips the bits of a binary string, performing a 1's complement operation.
 *
 * @param binary The binary string to be flipped.
 * @return The success status of the operation.
 */
int flip_negative(char binary[]);

/**
 * @brief Adds one to a binary string to perform 2's complement for negative numbers.
 *
 * This function adds one to a binary string, performing a carry operation to achieve
 * 2's complement conversion for negative numbers.
 *
 * @param binary The binary string to which one is added.
 * @return The success status of the operation.
 */
int add_one(char binary[]);

/**
 * @brief Adds a new label entry to the label table.
 *
 * This function adds a new label entry to the label table with its details such as name, line number,
 * address, and whether it is an instruction.
 *
 * @param labelTable Pointer to the label table array.
 * @param labelName The name of the label.
 * @param labelCount Pointer to the label count.
 * @param counterValue The counter value associated with the label.
 * @param isInstruction Flag indicating if the label is associated with an instruction.
 * @return The success status of adding the label.
 */
int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int isInstruction);

/**
 * @brief Adds the first operand's word to the instruction image.
 *
 * This function constructs and adds the first operand's word to the instruction image
 * by encoding the source operand's addressing mode, opcode, and ARE fields.
 *
 * @param icImage Pointer to the instruction image array.
 * @param op Pointer to the operation structure.
 * @param ic Pointer to the instruction counter.
 * @return The success status of adding the word.
 */
int add_first_op_word(char ***icImage, operation *op, int *ic);

/**
 * @brief Determines the opcode value for a given operation name.
 *
 * This function maps an operation name to its corresponding opcode value.
 *
 * @param opname The operation name.
 * @return The opcode value for the operation, or -1 if not found.
 */
int is_operation(char *opname);

/**
 * @brief Checks the formatting of a string line and identifies the indices of the string content.
 *
 * This function examines the formatting of a string line to ensure it is enclosed within double quotes.
 * It also identifies the indices of the start and end of the string content within the line.
 *
 * @param stringLine The string line to be checked.
 * @param index1 Pointer to store the index of the start of the string content.
 * @param index2 Pointer to store the index of the end of the string content.
 * @return The validation status of the string formatting.
 */
int check_string_formatting(char* stringLine, int *index1, int *index2);

/**
 * @brief Analyzes a string declaration and converts it to an array of integers.
 *
 * This function analyzes a string declaration and converts the string content into an array of integers.
 * It validates the string format, handles escape characters, and converts each character to its ASCII value.
 *
 * @param stringConverted Pointer to store the converted integer array.
 * @param stringLine The string line to be analyzed and converted.
 * @return The number of characters converted, or an error status if conversion fails.
 */
int analyze_string(int** stringConverted, char* stringLine);

/**
 * @brief Analyzes a data declaration and converts parameters to an array of integers.
 *
 * This function analyzes a data declaration, extracts and validates parameters, and converts them to an array of integers.
 *
 * @param token Pointer to the token containing the data parameters.
 * @param params Pointer to store the converted integer array.
 * @return The success status of analyzing and converting the data.
 */
int analyze_data(char** token, int** params);

/**
 * @brief Adds data words to the data image.
 *
 * This function converts and adds data parameters to the data image as binary words.
 *
 * @param dcImage Pointer to the data image array.
 * @param params Pointer to the array of data parameters.
 * @param dc Pointer to the data counter.
 * @param paramCnt The number of parameters.
 * @return The success status of adding the data words.
 */
int add_data_word(char ***dcImage, int *params, int *dc, int paramCnt);

/**
 * @brief Finds an extent label in the extent label list.
 *
 * This function searches the extent label list for a label with a given name and returns its node.
 *
 * @param head Pointer to the head of the extent label list.
 * @param labelName The name of the label to be found.
 * @return The pointer to the extent label node, or NULL if not found.
 */
extentlabel* find_extent_label(extentlabel* head, char* labelName);

/**
 * @brief Adds an extent label entry to the extent label list.
 *
 * This function adds an extent label entry to the extent label list with its details, such as name and type.
 *
 * @param head Pointer to the head of the extent label list.
 * @param token Pointer to the token containing the label name.
 * @param type The type of the extent label (EXTERNAL or ENTRY).
 * @return The success status of adding the extent label.
 */
int add_extent_label(extentlabel** head, char** token, int type);

/**
 * @brief Checks if a token is a label declaration.
 *
 * This function checks if a token ends with a colon, indicating a label declaration.
 *
 * @param token Pointer to the token to be checked.
 * @param labelName Pointer to store the extracted label name.
 * @param line Pointer to the line containing the token.
 * @return The label status (TRUE if label declaration, FALSE otherwise).
 */
int is_label(char **token, char *labelName, char *line);

/**
 * @brief Finds a label in the label table.
 *
 * This function searches the label table for a label with a given name and returns its entry.
 *
 * @param labelTable Pointer to the label table array.
 * @param labelName The name of the label to be found.
 * @param labelCount The number of labels in the table.
 * @return The pointer to the label entry, or NULL if not found.
 */
label* find_label(label *labelTable, char *labelName, int labelCount);

/**
 * @brief Adds operand words to the instruction image.
 *
 * This function converts and adds operand parameters to the instruction image as binary words.
 *
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Pointer to the label table array.
 * @param op Pointer to the operation structure.
 * @param ic Pointer to the instruction counter.
 * @param labelCount The number of labels in the label table.
 * @return The success status of adding the operand words.
 */
int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount);

/**
 * @brief Gets the value of an operand and updates its attributes.
 *
 * This function analyzes the token to determine the type of operand (numeric, register, or label)
 * and updates the operand structure accordingly.
 *
 * @param op Pointer to the operand structure to be updated.
 * @param token Pointer to the token containing the operand information.
 * @return The success status of getting the operand value.
 */
int get_operand_value(operand* op, char* token);

/**
 * @brief Determines the value of an operand based on its addressing mode.
 *
 * This function determines the value of an operand based on its addressing mode and updates the provided value.
 *
 * @param labelTable Pointer to the label table array.
 * @param operandd Pointer to the operand structure.
 * @param val Pointer to store the operand value.
 * @param labelCount The number of labels in the label table.
 * @return The success status of determining the operand value.
 */
int get_type_val(label **labelTable, operand *operandd, int *val, int labelCount);

/**
 * @brief Creates a binary word for a pair of register values.
 *
 * This function constructs a binary word that represents a pair of register values for a combined instruction.
 *
 * @param binary Pointer to the buffer to store the binary word.
 * @param sourceVal The source register value.
 * @param destVal The destination register value.
 * @return The success status of creating the binary register word.
 */
int get_register_word(char* binary, int sourceVal, int destVal);

/**
 * @brief Creates a binary word for an isolated operand.
 *
 * This function constructs a binary word that represents an isolated operand value.
 *
 * @param operandd Pointer to the operand structure.
 * @param binary Pointer to the buffer to store the binary word.
 * @param val The operand value.
 * @return The success status of creating the binary isolated operand word.
 */
int get_isolated_word(operand *operandd, char* binary, int val);

/**
 * @brief Sets the source and destination operands for an operation.
 *
 * This function sets the source and destination operands for an operation based on the number of operands provided.
 *
 * @param op Pointer to the operation structure.
 * @param operand1 Pointer to the first operand structure.
 * @param operand2 Pointer to the second operand structure.
 * @return The success status of setting the operands.
 */
int set_operands(operation *op, operand *operand1, operand *operand2);

/**
 * @brief Determines the count of parameters separated by commas in a line.
 *
 * This function counts the number of parameters separated by commas in a line.
 *
 * @param line Pointer to the line to be analyzed.
 * @return The count of parameters.
 */
int get_comma_param_cnt(char* line);

/**
 * @brief Checks if a token corresponds to an extent instruction.
 *
 * This function checks if a token corresponds to a ".entry" or ".extern" instruction.
 *
 * @param token Pointer to the token to be checked.
 * @return The instruction type (EXTENTRY if ".entry", EXTEXTERN if ".extern", or FALSE if neither).
 */
int is_extent_instruction(char* token);

/**
 * @brief Checks if a token corresponds to a data or string instruction.
 *
 * This function checks if a token corresponds to a ".data" or ".string" instruction.
 *
 * @param token Pointer to the token to be checked.
 * @return The instruction type (DATADATA if ".data", DATASTRING if ".string", or FALSE if neither).
 */
int is_datastring_instruction(char* token);

/**
 * @brief Calls the appropriate analyzer for data or string instructions.
 *
 * This function determines the type of data or string instruction and calls the respective analyzer.
 *
 * @param lineToken Pointer to the token containing the line.
 * @param params Pointer to store the converted integer array.
 * @param orgLineToken Original token before any changes.
 * @param commandCode The command code indicating the instruction type.
 * @return The result of analyzing the instruction and its parameters.
 */
int call_datastring_analyzer(char** lineToken, int** params, char* orgLineToken, int commandCode);

/**
 * @brief Retrieves a pointer to the next parameter after a specific character.
 *
 * This function retrieves a pointer to the next parameter after a specific character in the line.
 *
 * @param orgLineToken Original token containing the line.
 * @param toFind The character to find.
 * @return Pointer to the next parameter after the specified character, or NULL if not found.
 */
char* get_param_pointer(char* orgLineToken, char toFind);

/**
 * @brief Initializes the destination operand sequence array for various opcodes.
 *
 * This function initializes the sequenceArray with destination operand sequences for different opcodes.
 *
 * @param sequenceArray The array to store the destination operand sequences.
 * @return The success status of array initialization.
 */
int set_sequence_array_dest(char sequenceArray[16][4]);

/**
 * @brief Initializes the source operand sequence array for various opcodes.
 *
 * This function initializes the sequenceArray with source operand sequences for different opcodes.
 *
 * @param sequenceArray The array to store the source operand sequences.
 * @return The success status of array initialization.
 */
int set_sequence_array_source(char sequenceArray[16][4]);

/**
 * @brief Handles extent marking in the source code.
 *
 * This function processes the extent marking in the source code and adds extent labels to the linked list.
 *
 * @param head Pointer to the extent label linked list.
 * @param token Pointer to the current token.
 * @param originalLine Pointer to the original line containing the extent marking.
 * @param commandCode The command code indicating the extent instruction type.
 * @return The success status of extent marking processing.
 */
int extent_handler(extentlabel** head, char** token, char* originalLine, int commandCode);

/**
 * @brief Handles data and string instructions in the source code.
 *
 * This function processes data and string instructions in the source code and adds data words to the dcImage.
 *
 * @param dcImage Pointer to the dcImage array.
 * @param token Pointer to the current token.
 * @param originalLine Pointer to the original line containing the data or string instruction.
 * @param dc Pointer to the data counter.
 * @param commandCode The command code indicating the data or string instruction type.
 * @return The success status of data or string instruction processing.
 */
int datastring_handler(char*** dcImage, char** token, char* originalLine, int* dc, int commandCode);

/**
 * @brief Handles operation instructions in the source code.
 *
 * This function processes operation instructions in the source code, validates operands, and adds operation words to the icImage.
 *
 * @param icImage Pointer to the icImage array.
 * @param labelTable Pointer to the label table array.
 * @param token Pointer to the current token.
 * @param originalLine Pointer to the original line containing the operation instruction.
 * @param ic Pointer to the instruction counter.
 * @param labelCount Pointer to the label count.
 * @param commandCode The command code indicating the operation instruction type.
 * @param sourceSequenceArray The array containing source operand sequences.
 * @param destSequenceArray The array containing destination operand sequences.
 * @return The success status of operation instruction processing.
 */
int operation_handler(char*** icImage, label** labelTable, char** token, char* originalLine, int* ic, int* labelCount, int commandCode, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);

/**
 * @brief Handles warning messages generated during assembly.
 *
 * This function handles and prints warning messages during the assembly process.
 *
 * @param warningCode The code indicating the type of warning.
 * @return The success status of warning handling.
 */
int fp_warning_handler(int warningCode);

/**
 * @brief Handles error messages generated during assembly.
 *
 * This function handles and prints error messages during the assembly process.
 *
 * @param errorCode The code indicating the type of error.
 * @return The error status.
 */
int fp_error_handler(int errorCode);

#endif /* FIRSTPASS_H */
