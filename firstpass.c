#include "firstpass.h"

/*------------------------------------------------------------- FUNCTIONS NOT IN USE RIGHT NOW, BUT NEED LATER ----------------------------------*/
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

/* ------------------------------------------------------------ FUNCTIONS IN USE BUT THAT RETURN WORTHLESS VALUES ---------------------------------- */

int error_handler(){
    printf("theres error!\n");
    return 0;
}

int add_extern_labels(){
    return 0;
}

/* ----------------------------------------------------------------- FUNCTIONS IN USE ------------------------------------------------------------------- */

/* what is counter type? maybe switch (*labelTable) with something more readable? */
// might break when number entered is negative
int convert_to_binary(char binary[], int number, int size){
    int isNegative;

    memset(binary, 0, size+1);
    isNegative = (number < 0) ? 1 : 0;
    if (isNegative == 1)
        number *= -1;    

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

    if (isNegative == 1){
        flip_negative(binary);
        add_one(binary);
    }
    return 0;
}

// make this nicer 
int flip_negative(char binary[]){
    int i;
    // flipping all bits 
    for (i = 0; i < strlen(binary); i++)
        binary[i] = (binary[i] == '0') ? '1' : '0';
   
    return TRUE;
}

/*a helper function to add 1 to the the binary repressentation for negative numbers. Part of the 2's complement steps*/
int add_one(char binary[]){
    int carry, i;
    carry = 1;
    i = strlen(binary) - 1;
    
    // adding one 
    while (carry == 1 && i >= 0) {
        if (binary[i] == '0') {
            binary[i] = '1';
            carry = 0;
        } 
        else
            binary[i] = '0';
        i--;
    }    
    return TRUE;
}

int add_label(label** labelTable, char labelName[], int* labelCount, int counterValue, int lineNum){
    int i;
    (*labelTable) = (label*)realloc((*labelTable), sizeof(label) * ((*labelCount)+1));

    if (find_label(*labelTable, labelName, *labelCount) != FALSE)
        return error_handler();
    
    strcpy((*labelTable)[*labelCount].name, labelName);
    (*labelTable)[*labelCount].line = lineNum;
    (*labelTable)[*labelCount].cnt.counterType = 0;
    (*labelTable)[*labelCount].cnt.address = counterValue+100; /*change that to a pointer outside of the function*/
    (*labelCount)++;
    return 0;
}


int write_first_word(char*** operationArray, operation* operationn, int* ic){
    FILE* obFile;
    
    char binary[13];  // 12 binary digits + 1 null terminator
    char sourceAM[4];
    char opcode[5];
    char destAM[4];
    
    // working only when theres no second operator
    convert_to_binary(sourceAM, operationn->sourceOperand->addrMode, 3);
    convert_to_binary(opcode, operationn->opcode, 4);
    convert_to_binary(destAM, operationn->destOperand->addrMode, 3);
    
    memset(binary, '\0', 13);
    strcat(binary, sourceAM);
    strcat(binary, opcode);
    strcat(binary, destAM);
    strcat(binary, "00");

    add_to_counterArray(operationArray, ic, binary);

    return 0;
}

int is_instruction_operation(char* opname){

    if (strcmp(opname, "mov") == 0)
        return Mov;
    else if (strcmp(opname, "cmp") == 0)
        return Cmp;
    else if (strcmp(opname, "add") == 0)
        return Add;
    else if (strcmp(opname, "sub") == 0)
        return Sub;
    else if (strcmp(opname, "lea") == 0)
        return Lea;
    else if (strcmp(opname, "clr") == 0)
        return Clr;
    else if (strcmp(opname, "not") == 0)
        return Not;
    else if (strcmp(opname, "inc") == 0)
        return Inc;
    else if (strcmp(opname, "dec") == 0)
        return Dec;
    else if (strcmp(opname, "jmp") == 0)
        return Jmp;
    else if (strcmp(opname, "bne") == 0)
        return Bne;
    else if (strcmp(opname, "red") == 0)
        return Red;
    else if (strcmp(opname, "prn") == 0)
        return Prn;
    else if (strcmp(opname, "jsr") == 0)
        return Jsr;
    else if (strcmp(opname, "rts") == 0)
        return Rts;
    else if (strcmp(opname, "stop") == 0)
        return Stop;

    return FALSE;
}

int string_handler(char* stringLine, int* stringConverted, int lineNum){
    int i;

    if (stringLine[0] != '\"' && stringLine[strlen(stringLine)-1] != '\"'){
        stringConverted[0] = error_handler();
        return TRUE;
    }

    stringLine++;
    stringLine[-1] = '\0'; // this line might cause problem: i need to check if it wont cause problem and if its even ok to do like that.
    stringLine[strlen(stringLine)-1] = '\0'; 

    for (i = 0; i < strlen(stringLine); i++)
        stringConverted[i] = stringLine[i];
    stringConverted[i] = 0;

    return i+1;
    // write_data(stringConverted, strlen(stringLine)+1, lineNum);
}

int data_handler(char** dataLine, int* params, int lineNum){
    char* toCheck;
    int commaCnt, paramCnt;

    paramCnt = commaCnt = 0;

/*     remove_spaces(*dataLine);
    toCheck = replace_commas(*dataLine); */

    /* token = strtok(toCheck, " "); */
    // params = (int*)calloc(paramCnt, sizeof(int)); // maybe allocate outside of the function? 

    while (*dataLine != NULL){
        if (**dataLine != ','){
            paramCnt++;
            if(strspn(*dataLine, "-+0123456789") == strlen(*dataLine)){
                params = (int*)realloc(params, sizeof(int) * paramCnt);
                params[paramCnt-1] = atoi(*dataLine);
            }
            else
                return error_handler(); // input isnt a number
        }
        else
            commaCnt++;

        *dataLine = strtok(NULL, DELIMITERS);
    }

/*     if (paramCnt - commaCnt != 1)
        return error_handler(); */
    return paramCnt;
}

int add_to_counterArray(char*** counterArray, int* counter, char* toAdd){
    (*counterArray) = (char**)realloc((*counterArray), sizeof(char*) * (*counter+1));
    (*counterArray)[*counter] = (char*)malloc(sizeof(char) * (strlen(toAdd)+1));

    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}

int write_data(char*** dataArray, int* params, int* dc, int paramCnt, int lineNum){
    int i;
    FILE* obFile;
    
    if (obFile == NULL) {
        printf("Failed to open prog.ob for writing!\n");
        return error_handler();
    }

    for (i = 0; i < paramCnt; i++) {
        char binary[13];  // 12 binary digits + 1 null terminator
        convert_to_binary(binary, params[i], 12);
        add_to_counterArray(dataArray, dc, binary);
        /* fprintf(obFile, "%s\n", binary); */
    }
    return TRUE;
}

int call_data_analyzer(char** token, int* convertedData, int commandCode, int lineNum){
    if (commandCode == _DATA)
        return data_handler(token, convertedData, lineNum);
    else if (commandCode == _STRING)
        return string_handler(*token, convertedData, lineNum);
    else if (commandCode == _EXTERN)
        return add_extern_labels();
    else if (commandCode == _ENTRY)
        return 1;
    else
        return FALSE;
    return TRUE;
}
/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 
int is_guidance_command(char* command){
    if (strcmp(command, ".data") == 0)
        return _DATA;
    else if (strcmp(command, ".string") == 0)
        return _STRING;
    else if (strcmp(command, ".extern") == 0)    
        return _EXTERN;
    else if (strcmp(command, ".entry") == 0)
        return _ENTRY;
    return 0; 
}
  
int is_label(char** token, char labelName[MAX_LABEL_LENGTH], char line[MAX_LINE_LENGTH]){
    if (LAST_CHARACTER(*token) == ':'){
        LAST_CHARACTER(*token) = '\0';
        strcpy(labelName, *token);
        *token = strtok(NULL, DELIMITERS);
        return 1;
    }
    if (is_guidance_command(labelName) != FALSE || is_instruction_operation(labelName) != FALSE)
        return error_handler();
    return 0;
}

FILE* open_file(char* filename, char* ending, char* mode){
    char foo[MAX_FILENAME_LENGTH]; // add max for file extension as well

    strcpy(foo, filename);
    strcat(foo, ending);
    FILE* file = fopen(foo, mode); // change that "a" to mode when done testing

    return file;
}

/*manager function. its job is to read from the input file, and report any errors that arise.*/
int read_input_file(FILE** sourceFile, char* filename, char ending[3], char line[MAX_LINE_LENGTH], int* lineNum){
    if (*lineNum == 0)    
        *sourceFile = open_file(filename, ending, "r"); 

    if (*sourceFile == NULL)
        return error_handler();

    if (fgets(line, MAX_LINE_LENGTH, *sourceFile) != NULL){
        (*lineNum)++;
        return 1;
    }

    return 0;
}

int find_label(label* labelTable, char* labelName, int labelCount){
    int i;

    for (i = 0; i < labelCount; i++)
        if (strcmp(labelTable[i].name, labelName) == 0)
            return labelTable[i].cnt.address;

    return FALSE;
}

// adds the operand words to the operation array
int add_operand_words(){
    return 0;
}

int get_combined_word(char* binary, int sourceVal, int destVal){
    memset(binary, '\0', 13);

    convert_to_binary(binary, sourceVal, 5);
    convert_to_binary(binary+5, destVal, 5);
    strcat(binary, "00"); // might not work sometime maybe? need to check this 

    return TRUE;
}

int get_one_word(char* binary, int val){
    memset(binary, '\0', 13);
    /* if (no operand)
	    return; */
    convert_to_binary(binary, val, 10);
    strcat(binary, "00");

    return TRUE;
}

// done
int set_operands(operation* operationn, operand* operand1, operand* operand2){
    if (operand1->addrMode == 0 && operand2->addrMode == 0){
        operationn->sourceOperand = NULL;
        operationn->destOperand = NULL;
    }
    else if (operand1->addrMode != 0 && operand2->addrMode != 0){
        operationn->sourceOperand = operand1;
        operationn->destOperand = operand2;
    }
    else{
        operationn->sourceOperand = NULL;
        operationn->destOperand = operand1;
    }
    
    return TRUE;
}

// done
int get_param_value(operand* op, char* token, int lineNum){
    char foo[1];

    token = strtok(NULL, DELIMITERS);
    if (token == NULL)
        op->addrMode = No_Operand;
    // not sure if can input + aswell, check that
    else if (strspn(token, "+-012345678") == strlen(token)){
        op->val.numericValue = atoi(token);
        op->addrMode = Immidiate;
    }
    else if (token[0] == '@' && token[1] == 'r'){
        foo[0] = token[2];
        if (strspn(foo, "012345678") == 1){
            op->val.regNum = atoi(foo);
            op->addrMode = Register;
        }
        // return error
    }
    else{
        strcpy(op->val.label, token);
        op->addrMode = Direct;
    }

    return TRUE;
}

// try cleaning up and bit and making this functions more readable and more focused 
int first_pass_invoker(char*** dataArray, char*** operationArray, FILE** amFile, label** labelTable, char* filename, int* dc, int* ic, int* labelCount){
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    char labelName[MAX_LABEL_LENGTH];
    int* counterPtr;
    int isLabel;
    int commandCode;
    int lineNum;

    *dc = *ic = *labelCount = lineNum = 0;

    while (read_input_file(amFile, filename,".am", originalLine, &lineNum) == 1){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);

        isLabel = is_label(&token, labelName, line);

        if ((commandCode = is_guidance_command(token)) != FALSE){
            int* convertedData;
            int paramCnt;
            convertedData = (int*)calloc(1, sizeof(int)); // move this to a different function

            token = strtok(NULL, DELIMITERS);
            paramCnt = call_data_analyzer(&token, convertedData, commandCode, lineNum); /* calls the data analyzer depending on the guidance operation */
            write_data(dataArray, convertedData, dc, paramCnt, lineNum);
            counterPtr = dc;
        }
        else if ((commandCode = is_instruction_operation(token)) != -1){
            operation operationn;
            operand operand1, operand2;
            operationn.opcode = commandCode;

            get_param_value(&operand1, token, lineNum);
            get_param_value(&operand2, token, lineNum);

            set_operands(&operationn, &operand1, &operand2);

            write_first_word(operationArray, &operationn, ic);
            add_operand_words(operationArray, labelTable, &operationn, ic, *labelCount);

            counterPtr = ic;
        }
        else
            return error_handler();

        if (commandCode != -1 && isLabel == TRUE)
            add_label(labelTable, labelName, labelCount, *counterPtr, lineNum);
    }

    // close files
    // free memory
    return TRUE;
}

// labels, ic dc, errors
int main(int argc, char** argv){
    FILE* amFile;
    label* labelTable;
    char filename[MAX_FILENAME_LENGTH];
    int dc, ic, labelCount;
    char** dcArray; 
    char** icArray;

    int i;
    for (i = 1; i < argc; i++){
        strcpy(filename, argv[i]);
        
        dcArray = (char**)malloc(sizeof(char*));
        icArray = (char**)malloc(sizeof(char*));
        labelTable = (label*)malloc(sizeof(label));

        first_pass_invoker(&dcArray, &icArray, &amFile, &labelTable, filename, &dc, &ic, &labelCount);

        printf("instructions words:\n");
        PRINTWORDS(icArray, ic, 0);
        printf("data words:\n");
        PRINTWORDS(dcArray, dc, ic);
    }
    // close files
    // free memory
    
}

/*-------------------------------------------------- TESTING FUNCTIONS --------------------------------------------*/

int PRINTWORDS(char** counterArray, int counter, int subCounter){
    int i;
    for (i = subCounter; i < counter+subCounter; i++){
        printf("address %d:", i+100);
        printf(" %s\n", counterArray[i-subCounter]);
    }
    return 0;
}