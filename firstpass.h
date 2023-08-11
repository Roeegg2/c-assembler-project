#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


// ------------------ need to go in utils.c ------------------ //
#define MAX_LINE_LENGTH 81
#define MAX_FILENAME_LENGTH 30

FILE* open_file(char* filename, char* ending, char* mode);
int read_input_file(FILE** sourceFile, char* filename, char ending[], char line[], int* lineNum);


// check all of these and make sure they are right!
#define SIZE_OF_WORD 12

#define MAX_LABEL_LENGTH 31
#define MAX_DATA_LENGTH 10 
#define MAX_OPERANDS 2

#define DELIMITERS " \t\n,"

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define ABSOLUTE "00"
#define RELOCATABLE "10"
#define EXTERNAL "01"

#define _WHITENOTE 1
#define _COMMA 2
#define _CHAR 3

#define _SOURCE 0
#define _DEST 1

#define _DATA 1
#define _STRING 2
#define _ENTRY 3
#define _EXTERN 4
#define _INSTRUCTION 6
#define _NONE 5

#define BOTH_ARE_NOT_NULL(sourceStatus, destStatus) (sourceStatus == 1 && destStatus == 1)
#define BOTH_ARE_REGISTER(op) (op->destOperand->addrMode == Register && op->sourceOperand->addrMode == Register)

#define LAST_CHARACTER(string) *(string+strlen(string)-1)
#define CHAR_TO_INT(c) (c - '0')

enum Opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum AddressingModes {No_Operand = 0, Immediate = 1, Direct = 3, Register = 5};
enum Errors {Unknown_Command = 0, Comma_Error, Extra_Comma, Double_Comma, Missing_Comma, 
    Invalid_Parameter_Data, Invalid_Source_Sequence, Invalid_Dest_Sequence, 
    Illegal_String_Declaration, Label_Already_Defined, Illegal_Comma_Name_First_Char, 
    Illegal_Comma_Name_Illegal_Chars, Illegal_Comma_Name_Saved_Word, Extent_Label_Already_Defined_Differently, 
    Label_Name_Length_Too_Long, Undefined_Register, Blank_Label_Declaration};
enum Warnings {Label_Points_At_ExternEntry = 0, Extent_Label_Already_Defined_Similarly};

typedef struct label{
    char labelName[MAX_LABEL_LENGTH];
    int line;
    int type;
    int address; 
} label;

typedef union operandValue{
    char labelName[MAX_LABEL_LENGTH];
    int regNum;
    int numericVal;
} operandValue;

typedef struct operand{
    int addrMode;
    operandValue val;
} operand;

typedef struct operation{
    int opcode;
    // char paramSequence[2];
    operand* sourceOperand;
    operand* destOperand;
} operation;

typedef struct extentlabel{
    char labelName[MAX_LABEL_LENGTH];
    struct{
        int* addr;
        int count;
    } address;
    int type;
    struct extentlabel* next;
} extentlabel;

int replace_commas(char* str);
int remove_spaces(char* str);
int error_handler();
int convert_to_binary(char binary[], int number, int size);
int add_label(label** labelTable, char* labelName, int* labelCount, int counterValue, int lineNum);
int add_first_op_word(char*** icImage, operation* op, int* ic);
int is_instruction_operation(char* opname);
int string_handler(char* stringLine, int* stringConverted, int lineNum);
int data_handler(char* dataLine, int* params, int lineNum);
int write_data(char*** dataArray, int* params, int* dc, int paramCnt, int lineNum);
int is_label(char** token, char* labelName, char* line, int lineNum);
int first_pass_invoker(char*** dcImage, char*** icImage, FILE** amFile, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount);
int add_operand_words(char*** icImage, label** labelTable, operation* op, int* ic, int labelCount);
int get_type_val(label** labelTable, operand* operandd, int* val, int labelCount);
int flip_negative(char binary[]);
int add_to_counterArray(char*** counterArray, int* counter, char* toAdd);
int PRINTWORDS(char** counterArray, int counter, int subCounter);
int PRINTEXTENT(extentlabel* head);
int PRINTLABEL(label* labelTable, int labelCount);
int add_one(char binary[]);
int get_register_word(char* binary, int sourceVal, int destVal);
int get_isolated_word(operand* operandd, char* binary, int val, int status);
int find_label(label* labelTable, char* labelName, int labelCount);
int get_extent_label_type(extentlabel* head, char* labelName);
int get_param_value(operand* op, char* token, int lineNum);
int set_operands(operation* operationn, operand* operand1, operand* operand2);
int guidance_information_analyzer(char* orginialLine, int** convertedData, int commandCode, int lineNum);
int is_guidance_label_command(char* command);
int is_guidance_information_command(char* command);
int add_extent_label(extentlabel** head, char** token, int type, int lineNum);
int get_comma_param_cnt(char* line, int lineNum);
int add_data_word(char ***dcImage, int *params, int *dc, int paramCnt, int lineNum);
int is_datastring_instruction(char* token);
int is_extent_instruction(char* token);
int call_analyzer(char** lineToken, int** params, char* orgLineToken, int lineNum, int commandCode);
int set_sequence_array_source(char sequenceArray[16][4]);
int set_sequence_array_dest(char sequenceArray[16][4]);
int check_param_sequence(char sequenceArray[16][4], operand* operandd, int opcode, int lineNum, int sourceOrDest);
int warning_handler(int warningCode, int lineNum);
// ---------------------- secondpass fucntions ---------------------- //

int secondpass_invoker(char*** dcImage, char*** icImage, label* labelTable, extentlabel** head, char* filename, int labelCount, int ic, int dc);
int map_labels(char*** icImage, label* labelTable, int* errorFound, int ic, int labelCount);
int write_ob_file(char** counterArray, label* labelTable, char* filename, int counter);
int write_extent_file(extentlabel* head, char* filename);
int complete_extent_data(char*** icImage, label* labelTable, extentlabel** head, int* errorFound, int ic, int labelCount);
int get_data_extern(char*** icImage, extentlabel** head, int* errorFound, int ic);
int get_data_entry(label* labelTable, extentlabel** head, int* errorFound, int labelCount);
int print_extern(extentlabel head, FILE* extFile);