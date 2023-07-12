/**
 * NOTES_REGARDING_CODE:
 * 1. have a problem, when passing some functions like add_label, the pointer declared is only in the function scope
 * 
 * NOTES_TO_SELF:
 * 1. function parameter sequence: pointers, arrays, simple data types 
 * (structs, weird built in datatypes, char, long, double, float, int, boolean-treated-ints)
 * 
*/

#include "firstpass.h"

FILE* open_file(char* fileName, int numOfEntries, char* ending){
    FILE* file;
    if (numOfEntries == 0)
        file = fopen("result.ext", "w"); /*switch here with file name and the right ending*/ 
    else
        file = fopen("result.ext", "a"); /*switch here with file name and the right ending*/ 
    return file;
}

/*adds a new label to the label table*/
label* add_label(label* labelHead, label* newLabel, char labelName[], int counterType, int lineNum){
    newLabel = (label*)malloc(sizeof(label*));
    
    if (labelHead == NULL){
        labelHead = newLabel;
        labelHead->next = NULL;
    }

    while (labelHead->next != NULL){
        if (strcmp(newLabel->name, labelName) == 0){
            return error_handler(Label_Already_Exists, lineNum);
        }
        labelHead = labelHead->next;
    }

    if (labelHead->next != NULL)
        labelHead->next = newLabel;
    strcpy(newLabel->name, labelName);
    newLabel->line = lineNum;
    newLabel->counterType = counterType;
    newLabel->type = 0; /* 0 = entry, 1 = external, 2 = data, 3 = code?*/
    return newLabel;
}

int error_handler(int errorCode, int lineNum){ /*in line num just input DC+IC-1*/ 
    switch(errorCode){
        case No_Valid_Operation:
            printf("Error: line %d: no such operation found.\n", lineNum);
            break;
        case Cant_Read_Line:
            printf("Error: line %d: couldn't read line.\n", lineNum);
            break;
        case Label_Already_Exists: 
            printf("Error: line %d: label already exists.\n", lineNum);
            return -1;    
    }
    return -1;
}

int is_instruction_operation(){
    return 0;
}
/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 
int is_guidance_operation(label* labelHead, label* newLable, char labelName[], char* token, int isLabel, int lineNum){
    if (strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0){
        if (isLabel == 1)
            add_label(labelHead, newLable, labelName, VAL_DC, lineNum); /* add the label to the label table, using DC, if the label already exists, print error */
        
        /* זהה את סוג הנתונים, קודד אותם בזיכרון, עדכן את מונה הנתונים דכ בהתאם לאורכם */
        return 1;
    }
    else if (strcmp(token, ".extern")){
        /*
        while (token != NULL){

        }    
        */    
        return 1;
    }
    if (strcmp(token, ".entry")){
        return 1;
    }
    return 0; 
}

/*The following function is a helper function. its job is to check whether or not the following line is
 a label decleration.
 returns 1 if it is a lable, 0 if its not. 
*/

int is_label(char** token, char labelName[], char originalLine[]){
    *token = strtok(originalLine, DELIMITERS);
    strcpy(labelName, *token);
    if (LAST_CHARACTER(labelName) == ':'){
        LAST_CHARACTER(labelName) = '\0';
        *token = strtok(NULL, DELIMITERS);
        return 1;
    }
    return 0;
}

/* The following function reads the next line from the .am (input file).
 It serves as the 'heart' of the program, calling and linking all the analasys and helper functions together.
*/
int input_handler(){
    FILE* amFile;
    label* labelHead;
    label* newLabel;

    char* token;
    char labelName[MAX_LABEL_LENGTH];
    char originalLine[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH];
    int lineNum, isLabel;
    int DC, IC;

    DC = IC = 0;
    lineNum = 0;
    amFile = fopen("result.am", "r");

    while (fgets(originalLine, MAX_LINE_LENGTH, amFile)){
        lineNum++;
        strcpy(line, originalLine);

        isLabel = is_label(&token, labelName, originalLine);
        /* if (is_guidance_operation(labelHead, newLabel, labelName, token, isLabel, lineNum) != 1 && is_instruction_operation() != 1){
            error_handler(No_Valid_Operation, lineNum);
        } */
        labelHead = add_label(labelHead, newLabel, labelName, VAL_IC, lineNum);
        printf("%s\n", labelHead->name);
    }
    return error_handler(Cant_Read_Line, lineNum);
}

int main(){
    input_handler();
    return 0;
}
