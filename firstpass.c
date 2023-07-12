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

int error_handler(int errorCode, int lineNum);

char* replace_commas(char* str){
    int i, j;
    char* result;

    if (str == NULL)
        return NULL;

    result = (char*)malloc(sizeof(char) * (strlen(str)+100));  /* NOTE: need allocate more memory for the modified string! */

    if (result == NULL) {
        printf("failed to allocate memory!");
        return NULL;
    }

    i = j = 0; /*i - index of current char in original string j- index of current char in the return string */
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            result[j++] = ' ';
            result[j++] = ',';
            result[j++] = ' ';
        } else {
            result[j++] = str[i];
        }
    }
    result[j] = '\0';

    return result;
}

void remove_spaces(char* str) {
    int i, j;

    if (str == NULL)
        return;

    for (i = 0, j = 0; str[i] != '\0'; i++) { /*i - index of current char j- index of current char that isnt a space character */
        if (str[i] != ' ') {
            str[j] = str[i];
            j++;
        }
    }
    str[j] = '\0';
}

// this function receives a data line, it analyses it, seperates each data field.
// NOTE: might want to remove char* result and just use the original dataLine
// NOTE: if needed a more deep analysis of the data, use the 'mycomp.c - maman22' function
int analyse_data(char dataLine[], int lineNum){
    char* toCheck;
    char* token;
    int* dataValues;
    int commaCnt, paramCnt;

    paramCnt = commaCnt = 0;

    remove_spaces(dataLine);
    toCheck = replace_commas(dataLine);

    token = strtok(toCheck, " ");

    while (token != NULL){
        if (*token != ','){
            paramCnt++;
            // write_data();
            dataValues = (int*)realloc(dataValues, sizeof(int) * paramCnt);
            dataValues[paramCnt-1] = atoi(token);
        }
        else
            commaCnt++;

        token = strtok(NULL, " ");
    }
    if (paramCnt - commaCnt != 1)
            return error_handler(_commaerror_, lineNum);
    for (int i = 0; i < paramCnt; i++)
        printf("wrote %d\n", dataValues[i]);

    return 0;
}

FILE* open_file(char* fileName, int numOfEntries, char* ending){
    FILE* file;
    if (numOfEntries == 0)
        file = fopen("result.ext", "w"); /*switch here with file name and the right ending*/ 
    else
        file = fopen("result.ext", "a"); /*switch here with file name and the right ending*/ 
    return file;
}

/*adds a new label to the label table*/
int add_label(label* labelTable, char labelName[], int counterType, int lineNum, int* labelIndex){
    int i;
    (*labelIndex)++;
    labelTable = (label*)realloc(labelTable, sizeof(label) * ((*labelIndex)+1));

    for (i = 0; i <= *labelIndex; i++)
        if (strcmp(labelTable[i].name, labelName) == 0)
            return error_handler(Label_Already_Exists, lineNum);
    
    strcpy(labelTable[*labelIndex].name, labelName);
    labelTable[*labelIndex].line = lineNum;
    labelTable[*labelIndex].type = 0;
    labelTable[*labelIndex].counterType = counterType; /*change that to a pointer outside of the function*/
    return 0;
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
            break;   
        case _commaerror_:
            printf("Error: line %d: comma error.\n", lineNum);
            break;
    }
    return -1;
}

int is_instruction_operation(){
    return 0;
}

/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 
int is_guidance_operation(label* labelTable, char labelName[], char* token, int isLabel, int lineNum, int* labelIndex){
    if (strcmp(token, ".data") == 0 || strcmp(token, ".string") == 0){
        if (isLabel == 1)
            add_label(labelTable, labelName, Data, lineNum, labelIndex); /* add the label to the label table, using DC, if the label already exists, print error */
        
        token = strtok(NULL, DELIMITERS);
        analyse_data(token, lineNum);
        /* זהה את סוג הנתונים, קודד אותם בזיכרון, עדכן את מונה הנתונים דכ בהתאם לאורכם */
        return 1;
    }
    else if (strcmp(token, ".extern")){
        token = strtok(NULL, DELIMITERS);
        while (token != NULL){
            add_label(labelTable, token, Extern, lineNum, labelIndex); /* add the label to the label table, using IC, if the label already exists, print error */
            token = strtok(NULL, DELIMITERS);
        }    
           
        return 1;
    }
    if (strcmp(token, ".entry")){
        return 1;
    }
    return 0; 
}

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

int input_handler(){
    FILE* amFile;
    label* labelTable;

    char* token;
    char labelName[MAX_LABEL_LENGTH];
    char originalLine[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH];
    int labelIndex;
    int lineNum, isLabel;
    int DC, IC;

    DC = IC = 0;
    lineNum = 0;
    labelIndex = -1;
    
    amFile = fopen("result.am", "r");
    labelTable = (label*)malloc(sizeof(label*));

    while (fgets(originalLine, MAX_LINE_LENGTH, amFile)){
        lineNum++;
        strcpy(line, originalLine);

        isLabel = is_label(&token, labelName, originalLine);
        if (is_guidance_operation(labelTable, labelName, token, isLabel, lineNum, &labelIndex) != 1 && is_instruction_operation() != 1){
            error_handler(No_Valid_Operation, lineNum);
        }
        printf("%s\n", labelTable[labelIndex].name);
    }
    return 0;
}

int main(){
    input_handler();
    return 0;
}