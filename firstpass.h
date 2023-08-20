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
enum FirstpassErrors {Unknown_Command = 4, Extra_Comma, Double_Comma, Missing_Comma, 
    Parameter_Not_Whole_Number, Parameter_Out_Of_Bounds, Invalid_Source_Sequence, Invalid_Dest_Sequence, 
    Illegal_String_Declaration, Label_Already_Defined, Extent_Label_Already_Defined_Differently, 
    Undefined_Register, Blank_Label_Declaration, Missing_Dest_Operand, 
    Missing_Source_Operand, Too_Many_Operands, Blank_Extent_Marking, Illegal_String_Char, Blank_DataString_Instruction};
enum Warnings {Label_Points_At_ExternEntry = 0, Extent_Label_Already_Defined_Similarly};

typedef struct operand{
    int addrMode;
    union {
        char labelName[MAX_LABEL_MACRO_LENGTH];
        int regNum;
        int numericVal;
    } val; 
} operand;

typedef struct operation{
    int opcode;
    operand* sourceOperand;
    operand* destOperand;
} operation;

/**
 * This function reads an assembly file, processes each line, and generates intermediate code.
 * It also maintains a label table and handles label declarations, instructions, and operations.
 *
 * @param dcImage Pointer to the data image array.
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Pointer to the label table array.
 * @param head Pointer to the extent label list.
 * @param mapHead Pointer to the map label list.
 * @param filename Name of the assembly file to be processed.
 * @param dc Pointer to the data counter.
 * @param ic Pointer to the instruction counter.
 * @param labelCount Pointer to the label count.
 * @param sourceSequenceArray Array of source operand sequence patterns.
 * @param destSequenceArray Array of destination operand sequence patterns.
 * @return The status of the function execution.
 */
int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, maplabel** mapHead, char* filename, int* dc, int* ic, int* labelCount, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);
/**
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
 * This function flips the bits of a binary string, performing a 1's complement operation.
 *
 * @param binary The binary string to be flipped.
 * @return The success status of the operation.
 */
int flip_negative(char binary[]);

/**
 * This function adds one to a binary string, performing a carry operation to achieve
 * 2's complement conversion for negative numbers.
 *
 * @param binary The binary string to which one is added.
 * @return The success status of the operation.
 */
int add_one(char binary[]);

/**
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
 * This function analyzes a string declaration and converts the string content into an array of integers.
 * It validates the string format, handles escape characters, and converts each character to its ASCII value.
 *
 * @param stringConverted Pointer to store the converted integer array.
 * @param stringLine The string line to be analyzed and converted.
 * @return The number of characters converted, or an error status if conversion fails.
 */
int analyze_string(int** stringConverted, char* stringLine);

/**
 * This function analyzes a data declaration, extracts and validates parameters, and converts them to an array of integers.
 *
 * @param token Pointer to the token containing the data parameters.
 * @param params Pointer to store the converted integer array.
 * @return The success status of analyzing and converting the data.
 */
int analyze_data(char** token, int** params);

/**
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
 * This function adds an extent label entry to the extent label list with its details, such as name and type.
 *
 * @param head Pointer to the head of the extent label list.
 * @param token Pointer to the token containing the label name.
 * @param type The type of the extent label (EXTERNAL or ENTRY).
 * @return The success status of adding the extent label.
 */
int add_extent_label(extentlabel** head, char** token, int type);

/**
 * This function adds an entry label to the map label list with its details, such as name and address.
 * 
 * @param mapHead Pointer to the head of the map label list.
 * @param currIc The current ic value.
*/

maplabel* add_maplabel(maplabel* mapHead, int currIc);
/**
 * This function checks if a token ends with a colon, indicating a label declaration.
 *
 * @param token Pointer to the token to be checked.
 * @param labelName Pointer to store the extracted label name.
 * @param line Pointer to the line containing the token.
 * @return The label status (TRUE if label declaration, FALSE otherwise).
 */
int is_label(char **token, char *labelName, char *line);

/**
 * This function converts and adds operand parameters to the instruction image as binary words.
 *
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Pointer to the label table array.
 * @param op Pointer to the operation structure.
 * @param ic Pointer to the instruction counter.
 * @param labelCount The number of labels in the label table.
 * @return The success status of adding the operand words.
 */
int add_operand_words(char ***icImage, label **labelTable, maplabel** mapHead, operation *op, int *ic, int labelCount);
/**
 * This function analyzes the token to determine the type of operand (numeric, register, or label)
 * and updates the operand structure accordingly.
 *
 * @param op Pointer to the operand structure to be updated.
 * @param token Pointer to the token containing the operand information.
 * @return The success status of getting the operand value.
 */
int get_operand_value(operand* op, char* token);

/**
 * This function determines the value of an operand based on its addressing mode and updates the provided value.
 *
 * @param labelTable Pointer to the label table array.
 * @param operandd Pointer to the operand structure.
 * @param val Pointer to store the operand value.
 * @param labelCount The number of labels in the label table.
 * @return The success status of determining the operand value.
 */
int get_right_val(label **labelTable, operand *operandd, int *val, int labelCount);

/**
 * This function constructs a binaruy word that represents a pair of register values for a combined instruction.
 *
 * @param binary Pointer to the buffer to store the binary word.
 * @param sourceVal The source register value.
 * @param destVal The destination register value.
 * @return The success status of creating the binary register word.
 */
int get_register_word(char* binary, int sourceVal, int destVal);

/**
 * This function constructs a binary word that represents an isolated operand value.
 *
 * @param operandd Pointer to the operand structure.
 * @param binary Pointer to the buffer to store the binary word.
 * @param val The operand value.
   @param destOrSource _SOURCE if the operand is source and _DEST if the operand is dest.
 * @return The success status of creating the binary isolated operand word.
 */
int get_isolated_word(operand *operandd, char* binary, int val, int destOrSource);

/**
 * This function sets the source and destination operands for an operation based on the number of operands provided.
 *
 * @param op Pointer to the operation structure.
 * @param operand1 Pointer to the first operand structure.
 * @param operand2 Pointer to the second operand structure.
 * @return The success status of setting the operands.
 */
int set_operands(operation *op, operand *operand1, operand *operand2);

/**
 * This function counts the number of parameters separated by commas in a line.
 *
 * @param line Pointer to the line to be analyzed.
 * @return The count of parameters.
 */
int get_comma_param_cnt(char* line);

/**
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
 * This function retrieves a pointer to the next parameter after a specific character in the line.
 *
 * @param orgLineToken Original token containing the line.
 * @param toFind The character to find.
 * @return Pointer to the next parameter after the specified character, or NULL if not found.
 */
char* get_param_pointer(char* orgLineToken, char toFind);

/**
 * This function initializes the sequenceArray with destination operand sequences for different opcodes.
 *
 * @param sequenceArray The array to store the destination operand sequences.
 * @return The success status of array initialization.
 */
int set_sequence_array_dest(char sequenceArray[16][4]);

/**
 * This function initializes the sequenceArray with source operand sequences for different opcodes.
 *
 * @param sequenceArray The array to store the source operand sequences.
 * @return The success status of array initialization.
 */
int set_sequence_array_source(char sequenceArray[16][4]);

/**
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
 * This function processes operation instructions in the source code, validates operands, and adds operation words to the icImage.
 *
 * @param icImage Pointer to the icImage array.
 * @param labelTable Pointer to the label table array.
 * @param mapHead Pointer to the map label linked list.
 * @param token Pointer to the current token.
 * @param originalLine Pointer to the original line containing the operation instruction.
 * @param ic Pointer to the instruction counter.
 * @param labelCount Pointer to the label count.
 * @param commandCode The command code indicating the operation instruction type.
 * @param sourceSequenceArray The array containing source operand sequences.
 * @param destSequenceArray The array containing destination operand sequences.
 * @return The success status of operation instruction processing.
 */
int operation_handler(char*** icImage, label** labelTable, maplabel** mapHead, char** token, char* originalLine, int* ic, int* labelCount, int commandCode, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);

/**
 * This function handles and prints warning messages during the assembly process.
 *
 * @param warningCode The code indicating the type of warning.
 * @return The success status of warning handling.
 */
int fp_warning_handler(int warningCode);

/**
 * This function handles and prints error messages during the assembly process.
 *
 * @param errorCode The code indicating the type of error.
 * @return The error status.
 */
int fp_error_handler(int errorCode);

#endif /* FIRSTPASS_H */
