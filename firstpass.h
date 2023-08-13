#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include "shared.h"

// check all of these and make sure they are right!
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

#define LAST_CHARACTER(string) *(string+strlen(string)-1)
#define CHAR_TO_INT(c) (c - '0')

enum Opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum AddressingModes {No_Operand = 0, Immediate = 1, Direct = 3, Register = 5};
enum FirstpassErrors {Unknown_Command = 0, Comma_Error, Extra_Comma, Double_Comma, Missing_Comma, 
    Invalid_Parameter_Data, Invalid_Source_Sequence, Invalid_Dest_Sequence, 
    Illegal_String_Declaration, Label_Already_Defined, Illegal_Comma_Name_First_Char, 
    Illegal_Comma_Name_Illegal_Chars, Illegal_Comma_Name_Saved_Word, Extent_Label_Already_Defined_Differently, 
    Label_Name_Length_Too_Long, Undefined_Register, Blank_Label_Declaration};
enum Warnings {Label_Points_At_ExternEntry = 0, Extent_Label_Already_Defined_Similarly};

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

int error_handler(int errorCode, int lineNum);
int warning_handler(int warningCode, int lineNum);

int analyze_data(char** token, int *params, int lineNum);
int analyze_string(char* stringLine, int* stringConverted, int lineNum);

int get_type_val(label** labelTable, operand* operandd, int* val, int labelCount); // GO OVER THIS AND REWRITE THE WHOLE MECHANISM THIS FUNCITON IS PART OF
int get_operand_value(operand* op, char* token, int lineNum);
int set_operands(operation* op, operand *operand1, operand *operand2);

int get_isolated_word(operand* operandd, char* binary, int val, int status);
int get_register_word(char* binary, int sourceVal, int destVal);
int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount);
int add_first_op_word(char ***icImage, operation *op, int *ic);
int add_are(char *binary, char *are); //might remove this function 
int get_operand_addrmode(operand *operandd, char *binary, int startingPoint);

int add_data_word(char*** dcImage, int* params, int* dc, int paramCnt, int lineNum);

int set_sequence_array_source(char sequenceArray[16][4]);
int set_sequence_array_dest(char sequenceArray[16][4]);
int get_comma_param_cnt(char* line, int lineNum);
int check_param_sequence(char sequenceArr[16][4], operand* operandd, int opcode, int lineNum, int sourceOrDest);
char* get_param_pointer(char* orgLineToken, char toFind);

int is_operation(char* opname);
int is_datastring_instruction(char* token);
int is_extent_instruction(char* token);
int call_datastring_analyzer(char** lineToken, int** params, char* orgLineToken, int lineNum, int commandCode);

int find_label(label* labelTable, char* labelName, int labelCount);
int is_label(char** token, char* labelName, char* line, int lineNum);
int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int lineNum);

int add_extent_label(extentlabel** head, char** token, int type, int lineNum);
int get_extent_label_type(extentlabel* head, char* labelName);

int add_one(char binary[]);
int flip_negative(char binary[]);
int convert_to_binary(char binary[], int number, int size);

int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount);


#endif /* FIRSTPASS_H */