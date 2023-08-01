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

int error_handler(int t){
    if (t == 0)
        printf("gotcha!\n");
    else
        printf("theres error!\n");

    return 0;
}

/* ----------------------------------------------------------------- FUNCTIONS IN USE ------------------------------------------------------------------- */

/*function that receives a string, a number and a size, and writes into the string the binary */
/* what is counter type? maybe switch (*labelTable) with something more readable? */
// might break when number entered is negative
int convert_to_binary(char binary[], int number, int size){
    int isNegative;

    memset(binary, '0', size);
    binary[size] = '\0';

    isNegative = (number < 0) ? 1 : 0;
    if (isNegative == 1)
        number *= -1;    

    int i = size - 1;
    while (number > 0) {
        binary[i] = '0' + number % 2;
        number /= 2;
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

int add_label(label** labelTable, char* labelName, int* labelCount, int counterValue, int lineNum){
    int i;
    (*labelTable) = (label*)realloc((*labelTable), sizeof(label) * ((*labelCount)+1));

    if (find_label(*labelTable, labelName, *labelCount) != FALSE)
        return error_handler(1);
    
    strcpy((*labelTable)[*labelCount].name, labelName);
    (*labelTable)[*labelCount].line = lineNum;
    (*labelTable)[*labelCount].address = counterValue+100; /*change that to a pointer outside of the function*/
    (*labelCount)++;
    return 0;
}

int convert_operand(operand* operandd, char* binary, int startingPoint){
    if (operandd != NULL)
        convert_to_binary(binary+startingPoint, operandd->addrMode, 3);
    else
        strcat(binary, "000");

    return 0;
}

int add_are(char* binary, char* are){
    strcat(binary, "00");
    return 0;
}

int add_first_op_word(char*** icImage, operation* op, int* ic){
    char binary[13];  // 12 binary digits + 1 null terminator

    memset(binary, '\0', 13);

    convert_operand(op->sourceOperand, binary, 0);
    convert_to_binary(binary+3, op->opcode, 4);
    convert_operand(op->destOperand, binary, 7);
    add_are(binary, ABSOLUTE);

    add_to_counterArray(icImage, ic, binary);

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
        stringConverted[0] = error_handler(1);
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
                return error_handler(1); // input isnt a number
        }
        else
            commaCnt++;

        *dataLine = strtok(NULL, DELIMITERS);
    }

/*     if (paramCnt - commaCnt != 1)
        return error_handler(1); */
    return paramCnt;
}

int add_to_counterArray(char*** counterArray, int* counter, char* toAdd){
    (*counterArray) = (char**)realloc((*counterArray), sizeof(char*) * (*counter+1));
    (*counterArray)[*counter] = (char*)malloc(sizeof(char) * MAX_LABEL_LENGTH));

    strcpy((*counterArray)[*counter], toAdd);
    (*counter)++;

    return TRUE;
}

int get_data_word(char*** dcImage, int* params, int* dc, int paramCnt, int lineNum){
    int i;
    FILE* obFile;
    
    if (obFile == NULL) {
        printf("Failed to open prog.ob for writing!\n");
        return error_handler(1);
    }

    for (i = 0; i < paramCnt; i++) {
        char binary[13];  // 12 binary digits + 1 null terminator
        convert_to_binary(binary, params[i], 12);
        add_to_counterArray(dcImage, dc, binary);
        /* fprintf(obFile, "%s\n", binary); */
    }
    return TRUE;
}

// want to change implementation
int add_extent_label(extentlabel** head, char** token, int type){
    extentlabel* newNode;
    extentlabel* temp;
    newNode = (extentlabel*)calloc(1, sizeof(extentlabel));

    if ((*head) == NULL)
        (*head) = newNode;

    temp = (*head);
    while (temp->next != NULL)
        temp = temp->next;

    if (temp != newNode)
        temp->next = newNode; 

    strcpy(newNode->labelName, *token);
    newNode->type = type;
    newNode->next = NULL;
}

int guidance_information_analyzer(char** token, int* convertedData, int commandCode, int lineNum){
    if (commandCode == _DATA)
        return data_handler(token, convertedData, lineNum);
    else if (commandCode == _STRING)
        return string_handler(*token, convertedData, lineNum);

    return FALSE;
}
/* NOTE: add_label is called from here, but it might be more organized and right to call it from the input_handler function.*/ 

int is_guidance_label_command(char* command){
    if (strcmp(command, ".extern") == 0)    
        return _EXTERN;
    else if (strcmp(command, ".entry") == 0)
        return _ENTRY;

    return FALSE;
}

int is_guidance_information_command(char* command){
    if (strcmp(command, ".data") == 0)
        return _DATA;
    else if (strcmp(command, ".string") == 0)
        return _STRING;

    return FALSE; 
}

int is_label(char** token, char* labelName, char* line){
    if (LAST_CHARACTER(*token) == ':'){
        LAST_CHARACTER(*token) = '\0';
        strcpy(labelName, *token);
        *token = strtok(NULL, DELIMITERS);
        return TRUE;
    }
    // checking if the label name is a saved word, which is illegal
    if (is_guidance_label_command(labelName) != FALSE || is_instruction_operation(labelName) != FALSE || is_guidance_information_command(labelName) != FALSE)
        return error_handler(1);
        
    return FALSE;
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
        return error_handler(1);

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
            return labelTable[i].address; // need to return the address plus 100

    return FALSE;
}

// adds the operand words to the operation array
int add_operand_words(char*** icImage, label** labelTable, operation* op, int* ic, int labelCount){
    int sourceVal, destVal;
    int sourceStatus, destStatus;
    char binary[31];

    sourceStatus = get_type_val(labelTable, op->sourceOperand, &sourceVal, labelCount);
    destStatus = get_type_val(labelTable, op->destOperand, &destVal, labelCount);

    if (BOTH_ARE_NOT_NULL(sourceStatus, destStatus) && BOTH_ARE_REGISTER(op)){
        get_register_word(binary, sourceVal, destVal);
        add_to_counterArray(icImage, ic, binary);
    }
    else{
        if (get_isolated_word(op->sourceOperand, binary, sourceVal, sourceStatus) == TRUE)
            add_to_counterArray(icImage, ic, binary);

        if (get_isolated_word(op->destOperand, binary, destVal, destStatus) == TRUE)
            add_to_counterArray(icImage, ic, binary);
    }

    return TRUE;
}

int get_type_val(label** labelTable, operand* operandd, int* val, int labelCount){
    if (operandd == NULL)
        return FALSE;

    switch(operandd->addrMode){
        case Direct:
            *val = find_label(*labelTable, operandd->val.labelName, labelCount);
            break;
        case Immediate:
            *val = operandd->val.numericVal;
            break;
        case Register:
            *val = operandd->val.regNum;
            break;
    }

    return TRUE;
}

int get_register_word(char* binary, int sourceVal, int destVal){
    memset(binary, '\0', 13);

    convert_to_binary(binary, sourceVal, 5);
    convert_to_binary(binary+5, destVal, 5);
    strcat(binary, "00"); // might not work sometime maybe? need to check this 

    return TRUE;
}

int get_isolated_word(operand* operandd, char* binary, int val, int status){
    if (status == FALSE)
        return FALSE;

    memset(binary, '\0', 13);
    if (operandd->addrMode == Register)
        get_register_word(binary, val, 0);
    else if (operandd->addrMode == Direct)
        strcpy(binary, operandd->val.labelName); // binary size is too small
    else{
        convert_to_binary(binary, val, 10);
        strcat(binary, "00");   
    }

    return TRUE;
}

int set_operands(operation* op, operand* operand1, operand* operand2){
    if (operand1->addrMode == 0 && operand2->addrMode == 0){
        op->sourceOperand = NULL;
        op->destOperand = NULL;
    }
    else if (operand1->addrMode != 0 && operand2->addrMode != 0){
        op->sourceOperand = operand1;
        op->destOperand = operand2;
    }
    else{
        op->sourceOperand = NULL;
        op->destOperand = operand1;
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
        op->val.numericVal = atoi(token);
        op->addrMode = Immediate;
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
        strcpy(op->val.labelName, token);
        op->addrMode = Direct;
    }

    return TRUE;
}

// try cleaning up and bit and making this functions more readable and more focused 
int first_pass_invoker(char*** dcImage, char*** icImage, FILE** amFile, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount){
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    char labelName[MAX_LABEL_LENGTH+1];
    int* counterPtr;
    int commandCode;
    int lineNum;
    int temp;
    int isLabel;

    *dc = *ic = *labelCount = lineNum = 0;

    while (read_input_file(amFile, filename,".am", originalLine, &lineNum) == 1){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);

        isLabel = is_label(&token, labelName, line);

        if ((commandCode = is_guidance_label_command(token)) != FALSE){
            token = strtok(NULL, DELIMITERS);
            add_extent_label(head, &token, commandCode);
        }

        else if ((commandCode = is_guidance_information_command(token)) != FALSE){
            int* convertedData;
            int paramCnt;
            convertedData = (int*)calloc(1, sizeof(int)); // move this to a different function

            token = strtok(NULL, DELIMITERS);

            paramCnt = guidance_information_analyzer(&token, convertedData, commandCode, lineNum); /* calls the data analyzer depending on the guidance operation */
            get_data_word(dcImage, convertedData, dc, paramCnt, lineNum);
            counterPtr = dc;
        }
        else if ((commandCode = is_instruction_operation(token)) != -1){
            operation op;
            operand operand1, operand2;
            op.opcode = commandCode;

            get_param_value(&operand1, token, lineNum);
            get_param_value(&operand2, token, lineNum);

            set_operands(&op, &operand1, &operand2);

            add_first_op_word(icImage, &op, ic);
            add_operand_words(icImage, labelTable, &op, ic, *labelCount);

            counterPtr = ic;
        }
        else
            return error_handler(1);

        if (isLabel == TRUE)// see question 1
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
    extentlabel* head;
    char filename[MAX_FILENAME_LENGTH];
    int dc, ic, labelCount;
    char** dcImage; 
    char** icImage;

    int i;
    for (i = 1; i < argc; i++){
        strcpy(filename, argv[i]);
        
        dcImage = (char**)malloc(sizeof(char*));
        icImage = (char**)malloc(sizeof(char*));

        labelTable = (label*)malloc(sizeof(label));
        head = NULL;

        first_pass_invoker(&dcImage, &icImage, &amFile, &labelTable, &head, filename, &dc, &ic, &labelCount);

        printf("ext/ent list:\n");
        PRINTEXTENT(head);
        printf("instructions words:\n");
        PRINTWORDS(icImage, ic, 0);
        printf("data words:\n");
        PRINTWORDS(dcImage, dc, ic);
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

int PRINTEXTENT(extentlabel* head){
    while (head != NULL){
        printf("label: %s, type: %d\n", head->labelName, head->type);
        head = head->next;
    }
    return 0;
}

/**NOTES TO SELF:
 * 1. if command is wrong but has a lable pointing at it, should i add the label or not?
*/
