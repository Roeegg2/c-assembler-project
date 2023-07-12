#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 82
#define MAX_LABEL_LENGTH 31
#define DELIMITERS " \t\n"

#define VAL_DC 0
#define VAL_IC 1

#define LAST_CHARACTER(string) *(string+strlen(string)-1)

enum opcodes {Mov = 0, Cmp, Add, Sub, Not, Clr, Lea, Inc, Dec, Jmp, Bne, Red, Prn, Jsr, Rts, Stop};
enum addressingModes {Immidiate = 1, Direct = 3, Register = 5};
enum errors {No_Valid_Operation = 0, Cant_Read_Line, Label_Already_Exists};

typedef struct label{
    char name[MAX_LABEL_LENGTH]; /*the name of the label, eg "MAIN"*/
    int line; /*the line the table was defined*/ 
    int type; /*the type of the table relocateable, */
    int counterType;
    struct label* next;
} label;

FILE* open_file(char* fileName, int numOfEntries, char* ending);
label* add_label(label* labelHead, label* newLabel, char labelName[], int counterType, int lineNum);
int error_handler(int errorCode, int lineNum);
int is_instruction_operator();
int is_guidance_operation(label* labelHead, label* newLabel, char labelName[], char* token, int isLabel, int lineNum);
int is_label(char** token, char labelName[], char originalLine[]);
int input_handler();