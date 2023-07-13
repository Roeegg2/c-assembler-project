#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 82
#define MAX_LABEL_LENGTH 31
// check this and change later!
#define MAX_DATA_LENGTH 10 
#define DELIMITERS " \t\n"
#define MAX_OPERANDS 2

#define LAST_CHARACTER(string) *(string+strlen(string)-1)

enum Opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum AddressingModes {Immidiate = 1, Direct = 3, Register = 5};
enum Errors {No_Valid_Operation = 0, Cant_Read_Line, Label_Already_Exists, _commaerror_};
enum TypeMarking {Data = 0, Instruction, Extern, Entry}; /*not sure about entry*/

typedef struct label{
    char name[MAX_LABEL_LENGTH]; /*the name of the label, eg "MAIN"*/
    int line; /*the line the table was defined*/ 
    int type; /*the type of the table relocateable, */
    int counterType;
} label;

/* typedef union operand{
    label* label1;
    int registerNum;
    int number;
} operand;

typedef union data{
    int number;
    char string[MAX_LINE_LENGTH];
} data; */

/* typedef struct instruction{
    int opcode;
    int sourceAddressingModes;
    int destAddressingModes;
} instruction;

typedef union parameter{
    union{
        char labelName[MAX_LABEL_LENGTH];
        int registerNum;
        int number;
    } operand;

    union{
        int number;
        char string[MAX_DATA_LENGTH];
    } data;
} parameter; */