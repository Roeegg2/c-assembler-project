#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_LINE_LENGTH 82
#define MAX_LABEL_LENGTH 31
#define MAX_FILENAME_LENGTH 30
// check this and change later!
#define MAX_DATA_LENGTH 10 
#define DELIMITERS " \t\n,"
#define MAX_OPERANDS 2
#define TRUE 1
#define FALSE 0
#define _DATA 1
#define _STRING 2
#define _ENTRY 3
#define _EXTERN 4
#define _INSTRUCTION 5


#define LAST_CHARACTER(string) *(string+strlen(string)-1)

enum Opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum AddressingModes {No_Operand = -1, Immidiate = 1, Direct = 3, Register = 5};
enum Errors {No_Valid_Operation = 0, Cant_Read_Line, Label_Already_Exists, _commaerror_};
enum TypeMarking {Data = 0, Instruction, Extern, Entry}; /*not sure about entry*/

typedef struct label{
    char name[MAX_LABEL_LENGTH]; /*the name of the label, eg "MAIN"*/
    int line; /*the line the table was defined*/ 
    int type; /*the type of the table relocateable, */
    int counterType;
} label;

typedef union operand{
    // int nonExistent : 1; // might want to remove that
    int registerNumber;
    int value;
    char label[MAX_LABEL_LENGTH+1];
} operand;
typedef struct operation{
    int opcode;
    int sourceAM; // source addressing mode
    int destAM; // destination addressing mode
    operand sourceOperand;
    operand destOperand;
} operation;

char* replace_commas(char* str);
void remove_spaces(char* str);
int write_operand_word();
int error_handler();
int* add_extern_labels();
int convert_to_binary(int number, char binary[], int size);
int add_label(label** labelTable, char labelName[], int* labelCount, int counterType, int lineNum);
int get_operand_type(operand* op, char* token, int lineNum);
int write_first_word(operation* operationn);
int is_instruction_operation(char* opname);
int string_handler(char* stringLine, int* stringConverted, int lineNum);
int data_handler(char** dataLine, int* params, int lineNum);
int write_data(int* params, int paramCnt, int lineNum);
int call_data_analyzer(char** token, int* convertedData, int commandCode, int lineNum);
int is_guidance_operation(char* operation);
int is_label(char** token, char labelName[MAX_LABEL_LENGTH], char line[MAX_LINE_LENGTH]);
FILE* open_file(char* filename, char* ending, char* mode);
int read_input_file(FILE** sourceFile, char* filename, char ending[3], char line[MAX_LINE_LENGTH], int* lineNum);
int first_pass_invoker(FILE** amFile, label** labelTable, char* filename, int* dc, int* ic);