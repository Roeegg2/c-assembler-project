/**
 * General note: go over and making implementaions of everything more clean, efficient and readable. That means:
 * 1. brake some functions down
 * 2. try to minimize code duplication, unifiying things that can be unified
 * 3. make sure the flow of the program is clear yet logical, to make sure you and the person reading the code will understand it.
 * 
 * NOTES_REGARDING_CODE:
 * 1. change ugly implementation of the whole writing to binary of .data and .string entries.
 * 
 * 
 * 
 * NOTES_TO_SELF:
 * 1. function parameter sequence: pointers, arrays, simple data types 
 * (structs, weird built in datatypes, char, long, double, float, int, boolean-treated-ints)
 * 
*/

#include "firstpass.h"

int error_handler();

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

/* need to check if there is a certain type of parameteres for each operation, and add error detection here or in is_instruction_operation */
int analyse_operands(char operandLine[], int lineNum) {
    char* toCheck;
    char* token;
    int commaCnt, paramCnt;

    paramCnt = commaCnt = 0;

    remove_spaces(operandLine);
    toCheck = replace_commas(operandLine);

    token = strtok(toCheck, " ");

    // call write_to_memory();

    return 0;
}

void convert_to_binary(int number, char binary[], int size) {
    int i = size - 1;
    while (number > 0) {
        binary[i] = '0' + number % 2;
        number /= 2;
        i--;
    }
    while (i >= 0) {
        binary[i] = '0';
        i--;
    }
}



void write_data_to_file(int* params, int paramCnt, int lineNum) {
    FILE* obFile;

    obFile = fopen("prog.ob", "w");
    if (obFile == NULL) {
        printf("Failed to open prog.ob for writing!\n");
        return;
    }

    for (int i = 0; i < paramCnt; i++) {
        char binary[13];  // 12 binary digits + 1 null terminator
        convert_to_binary(params[i], binary, 12);
        fprintf(obFile, "%s\n", binary);
    }

    fclose(obFile);
}




// this function receives a data line, it analyses it, seperates each data field.
// NOTE: might want to remove char* result and just use the original dataLine
// NOTE: if needed a more deep analysis of the data, use the 'mycomp.c - maman22' function
// Maybe turn this into comma-handler/param-analyser

/* seperate and isolate each parameter, and return a string arary with all the parameters */
int data_handler(char* token, int lineNum){
    char* toCheck;
    int* params;
    int commaCnt, paramCnt;

    paramCnt = commaCnt = 0;

    /* remove_spaces(dataLine);
    toCheck = replace_commas(dataLine); */

    /* token = strtok(toCheck, " "); */
    params = (int*)calloc(paramCnt, sizeof(int));

    while (token != NULL){
        if (*token != ','){
            paramCnt++;

            if(strspn(token, "-+0123456789") == strlen(token)){
                params = (int*)realloc(params, sizeof(int) * paramCnt);
                params[paramCnt-1] = atoi(token);
            }
            else
                error_handler(); // input isnt a number
        }
        else
            commaCnt++;

        token = strtok(NULL, DELIMITERS);
    }

    write_data_to_file(params, paramCnt, lineNum);

    return 0;
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

int error_handler(){
    return 0;
}

int get_operand_type(operand* op, char* token, int lineNum){
    char foo[1];

    token = strtok(NULL, DELIMITERS);
    if (token == NULL)
        return No_Operand;
    // not sure if can input + aswell, check that
    else if (strspn(token, "+-012345678") == strlen(token)){
        op->value = atoi(token);
        return Immidiate;
    }
    else if (token[0] == '@' && token[1] == 'r'){
        foo[1] = token[2];
        if (strspn(foo, "012345678") == strlen(foo)){
            op->registerNumber = atoi(foo);
            return Register;
        }
        // return error
    }

    strcpy(op->label, token);
    return Direct;
}

int write_first_word(command* currCommand){
    FILE* obFile;

    obFile = fopen("prog.ob", "a");

    char sourceAddressingMode[4];
    char opcode[5];
    char destAddressingMode[4];
    convert_to_binary(currCommand->sourceAddressingMode, sourceAddressingMode, 3);
    convert_to_binary(currCommand->opcode, opcode, 4);
    convert_to_binary(currCommand->destAddressingMode, destAddressingMode, 3);

    fprintf(obFile, "%s%s%s%s\n", sourceAddressingMode, opcode, destAddressingMode, "00"); // 00 for ARE, need to change that
    return 0;
}

int analyse_operand(command* currCommand, char* token, int lineNum){
    currCommand->sourceAddressingMode = get_operand_type(&(currCommand->sourceOperand), token, lineNum);
    currCommand->destAddressingMode = get_operand_type(&(currCommand->destOperand), token, lineNum);
    write_first_word(currCommand);
}

int is_instruction_operation(label* labelTable, char* token, char labelName[], int* labelIndex, int isLabel, int lineNum){
    command currCommand;

    if (isLabel == 1)
        add_label(labelTable, labelName, Instruction, lineNum, labelIndex);

    if (strcmp(token, "mov") == 0)
        currCommand.opcode = Mov;
    else if (strcmp(token, "cmp"))
        currCommand.opcode = Cmp;
    else if (strcmp(token, "add") == 0)
        currCommand.opcode = Add;
    else if (strcmp(token, "sub") == 0)
        currCommand.opcode = Sub;
    else if (strcmp(token, "lea") == 0)
        currCommand.opcode = Lea;
    else if (strcmp(token, "clr") == 0)
        currCommand.opcode = Clr;
    else if (strcmp(token, "not") == 0)
        currCommand.opcode = Not;
    else if (strcmp(token, "inc") == 0)
        currCommand.opcode = Inc;
    else if (strcmp(token, "dec") == 0)
        currCommand.opcode = Dec;
    else if (strcmp(token, "jmp") == 0)
        currCommand.opcode = Jmp;
    else if (strcmp(token, "bne") == 0)
        currCommand.opcode = Bne;
    else if (strcmp(token, "red") == 0)
        currCommand.opcode = Red;
    else if (strcmp(token, "prn") == 0)
        currCommand.opcode = Prn;
    else if (strcmp(token, "jsr") == 0)
        currCommand.opcode = Jsr;
    else if (strcmp(token, "rts") == 0)
        currCommand.opcode = Rts;
    else if (strcmp(token, "stop") == 0)
        currCommand.opcode = Stop;
    else
        return error_handler(No_Valid_Operation, lineNum);

    analyse_operand(&currCommand, token, lineNum);
    return 0;
}

int string_handler(char* stringLine, int lineNum){
    if (stringLine[0] != '\"' && stringLine[strlen(stringLine)-1] != '\"')
        return error_handler();

    stringLine++;
    stringLine[-1] = '\0'; // this line might cause problem: i need to check if it wont cause problem and if its even ok to do like that.
    stringLine[strlen(stringLine)-1] = '\0'; 

    int stringConverted[strlen(stringLine)+1], i;
    for (i = 0; i < strlen(stringLine); i++)
        stringConverted[i] = stringLine[i];
    stringConverted[i] = 0;
    write_data_to_file(stringConverted, strlen(stringLine)+1, lineNum);
}

/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 
int is_guidance_operation(label* labelTable, char* token, char labelName[], int* labelIndex, int isLabel, int lineNum){
    if (strcmp(token, ".data") == 0){
        if (isLabel == 1)
            add_label(labelTable, labelName, Data, lineNum, labelIndex); /* add the label to the label table, using DC, if the label already exists, print error */
        
        token = strtok(NULL, DELIMITERS);
        data_handler(token, lineNum);
        /* זהה את סוג הנתונים, קודד אותם בזיכרון, עדכן את מונה הנתונים דכ בהתאם לאורכם */
        return 1;
    }
    else if (strcmp(token, ".string") == 0){
        if (isLabel == 1)
            add_label(labelTable, labelName, Data, lineNum, labelIndex);

        token = strtok(NULL, DELIMITERS);
        string_handler(token, lineNum); // this might cause problems because im passing token and modifying it in the function, could be affcting token in the main function.
        return 0;
    }
    else if (strcmp(token, ".extern") == 0){
        token = strtok(NULL, DELIMITERS);
        while (token != NULL){
            // add to the list of yet unknown labels!
            add_label(labelTable, token, Extern, lineNum, labelIndex); /* add the label to the label table, using IC, if the label already exists, print error */
            token = strtok(NULL, DELIMITERS);
        }    
        return 1;
    }
    else if (strcmp(token, ".entry") == 0){
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

/*the core of the first pass. This function*/
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
        if (is_guidance_operation(labelTable, token, labelName, &labelIndex, isLabel, lineNum) != 1){
            if (is_instruction_operation(labelTable, token, labelName, &labelIndex, isLabel, lineNum) != 1){
                error_handler();
            }
        }
    }
    fclose(amFile);
    return 0;
}

int main(){
    input_handler();
    return 0;
}