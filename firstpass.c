/**
 * NOTES:
 * Things left to implement:
 * 1.  
*/

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
int write_operand_word(){
    return 0;
}

int error_handler(){
    return 0;
}

int* add_extern_labels(){
    /* token = strtok(NULL, DELIMITERS);
    while (token != NULL){
            // add to the list of yet unknown labels!
        add_label_use(labelTable, token, Extern, lineNum, labelCount); // add the label to the label table, using IC, if the label already exists, print error
        token = strtok(NULL, DELIMITERS);
    } */
    int* ptr;
    ptr = (int*)malloc(sizeof(int));
    return ptr;
}

/* ----------------------------------------------------------------- FUNCTIONS IN USE ------------------------------------------------------------------- */

/* what is counter type? maybe switch (*labelTable) with something more readable? */
int convert_to_binary(int number, char binary[], int size){
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
    return 0;
}

int add_label(label** labelTable, char labelName[], int* labelCount, int counterType, int lineNum){
    int i;
    (*labelTable) = (label*)realloc((*labelTable), sizeof(label) * ((*labelCount)+1));

    for (i = 0; i <= *labelCount; i++)
        if (strcmp((*labelTable)[i].name, labelName) == 0)
            return error_handler(Label_Already_Exists, lineNum);
    
    strcpy((*labelTable)[*labelCount].name, labelName);
    (*labelTable)[*labelCount].line = lineNum;
    (*labelTable)[*labelCount].type = 0;
    (*labelTable)[*labelCount].counterType = counterType; /*change that to a pointer outside of the function*/
    (*labelCount)++;
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
        foo[0] = token[2];
        if (strspn(foo, "012345678") == 1){
            op->registerNumber = atoi(foo);
            return Register;
        }
        // return error
    }

    strcpy(op->label, token);
    return Direct;
}

int write_first_word(operation* operationn){
    FILE* obFile;

    obFile = open_file("prog", ".ob", "a");

    char sourceAM[4];
    char opcode[5];
    char destAM[4];

    memset(sourceAM, 0, sizeof(sourceAM));
    memset(opcode, 0, sizeof(opcode));
    memset(destAM, 0, sizeof(destAM));

    convert_to_binary(operationn->sourceAM, sourceAM, 3);
    convert_to_binary(operationn->opcode, opcode, 4);
    convert_to_binary(operationn->destAM, destAM, 3);

    fprintf(obFile, "%s%s%s%s\n", sourceAM, opcode, destAM, "00"); // 00 for ARE, need to change that
    fclose(obFile);

    return 0;
}

int is_instruction_operation(char* opname){

    if (strcmp(opname, "mov") == 0)
        return Mov;
    else if (strcmp(opname, "cmp"))
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

    /* remove_spaces(dataLine);
    toCheck = replace_commas(dataLine); */

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

    return paramCnt;
}

int write_data(int* params, int paramCnt, int lineNum){
    int i;
    FILE* obFile;
    
    if (*params == -1)
        return FALSE;
    
    obFile = open_file("prog", ".ob", "a");
    if (obFile == NULL) {
        printf("Failed to open prog.ob for writing!\n");
        return error_handler();
    }

    for (i = 0; i < paramCnt; i++) {
        char binary[13];  // 12 binary digits + 1 null terminator
        convert_to_binary(params[i], binary, 12);
        fprintf(obFile, "%s\n", binary);
    }

    fclose(obFile);
    return TRUE;
}

int call_data_analyzer(char** token, int* convertedData, int commandCode, int lineNum){
    if (commandCode == _DATA)
        return data_handler(token, convertedData, lineNum);
    else if (commandCode == _STRING)
        return string_handler(*token, convertedData, lineNum);
    else if (commandCode == _EXTERN)
        add_extern_labels();
    else if (commandCode == _ENTRY)
        return 1;
    else
        return FALSE;
    return TRUE;
}
/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 
int is_guidance_operation(char* operation){
    if (strcmp(operation, ".data") == 0)
        return _DATA;
    else if (strcmp(operation, ".string") == 0)
        return _STRING;
    else if (strcmp(operation, ".extern") == 0)    
        return _EXTERN;
    else if (strcmp(operation, ".entry") == 0)
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

int first_pass_invoker(FILE** amFile, label** labelTable, char* filename, int* dc, int* ic){
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    char labelName[MAX_LABEL_LENGTH];
    int isLabel;
    int labelCount;
    int commandCode;
    int lineNum;

    *dc = *ic = labelCount = lineNum = 0;

    while (read_input_file(amFile, filename,".am", originalLine, &lineNum) == 1){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);
        isLabel = is_label(&token, labelName, line);

        if ((commandCode = is_guidance_operation(token)) != FALSE){
            int* convertedData;
            int paramCnt;
            convertedData = (int*)calloc(1, sizeof(int)); // move this to a different function

            token = strtok(NULL, DELIMITERS);
            paramCnt = call_data_analyzer(&token, convertedData, commandCode, lineNum); /* calls the data analyzer depending on the guidance operation */
            write_data(convertedData, paramCnt, lineNum); // might be a problem here becasue of sizeof() on pointer and not array
        }
        else if ((commandCode = is_instruction_operation(token)) != -1){
            operation operationn;

            operationn.opcode = commandCode;
            operationn.sourceAM = get_operand_type(&(operationn.sourceOperand), token, lineNum);
            operationn.destAM = get_operand_type(&(operationn.destOperand), token, lineNum);

            write_first_word(&operationn);
            write_operand_word();
        }
        else
            return error_handler();

        if (commandCode != -1 && isLabel == TRUE)
            add_label(labelTable, labelName, &labelCount, 0, lineNum);
    }
    // close files
    // free memory
    return TRUE;
}
// labels, ic dc, errors
int main() {
    FILE* amFile;
    label* labelTable;
    char filename[MAX_FILENAME_LENGTH];
    int dc, ic;

    labelTable = (label*)malloc(sizeof(label));

    strcpy(filename, "result");
    first_pass_invoker(&amFile, &labelTable, filename, &dc, &ic);

    // close files
    // free memory
}