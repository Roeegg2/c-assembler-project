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

int fp_error_handler(int errorCode, int lineNum);
int fp_warning_handler(int warningCode, int lineNum);

int operation_handler(char*** icImage, label** labelTable, char** token, char* originalLine, int* ic, int* labelCount, int commandCode, int lineNum, char sourceSequenceArray[16][4], char destSequenceArray[16][4]);
int datastring_handler(char*** dcImage, char** token, char* originalLine, int* dc, int commandCode, int lineNum);
int extent_handler(extentlabel** head, char** token, char* originalLine, int commandCode, int lineNum);

int analyze_data(char** token, int** params, int lineNum);
int analyze_string(int** stringConverted, char* stringLine, int lineNum);

int get_type_val(label** labelTable, operand* operandd, int* val, int labelCount); /* GO OVER THIS AND REWRITE THE WHOLE MECHANISM THIS FUNCITON IS PART OF */
int get_operand_value(operand* op, char* token, int lineNum);
int set_operands(operation* op, operand *operand1, operand *operand2);

int get_isolated_word(operand* operandd, char* binary, int val, int status);
int get_register_word(char* binary, int sourceVal, int destVal);
int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount);
int add_first_op_word(char ***icImage, operation *op, int *ic);
int add_are(char *binary, char *are); /* might remove this function  */

int add_data_word(char*** dcImage, int* params, int* dc, int paramCnt, int lineNum);

int set_sequence_array_source(char sequenceArray[16][4]);
int set_sequence_array_dest(char sequenceArray[16][4]);
int get_comma_param_cnt(char* line, int lineNum);
int check_param_sequence(char sequenceArr[16][4], operand* operandd, int opcode, int lineNum, int sourceOrDest);
char* get_param_pointer(char* orgLineToken, char toFind);

int call_datastring_analyzer(char** lineToken, int** params, char* orgLineToken, int lineNum, int commandCode);

int is_label(char** token, char* labelName, char* line, int lineNum);
int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int isData, int lineNum);

int add_extent_label(extentlabel** head, char** token, int type, int lineNum);

int add_one(char binary[]);
int flip_negative(char binary[]);

int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount);


#endif /* FIRSTPASS_H */
