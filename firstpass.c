#include "firstpass.h"
#include "shared.h"
#include "utils.h"

/*function that receives a string, a number and a size, and writes into the string the binary */
/* what is counter type? maybe switch (*labelTable) with something more readable? */
/* might break when number entered is negative */
int convert_to_binary(char binary[], int number, int size){
    int isNegative, i;

    memset(binary, '0', size);
    binary[size] = '\0';

    isNegative = (number < 0) ? 1 : 0;
    if (isNegative == 1)
        number *= -1;

    i = size - 1;
    while (number > 0){
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

/* make this nicer */
int flip_negative(char binary[]){
    int i;
    /* flipping all bits */
    for (i = 0; i < strlen(binary); i++)
        binary[i] = (binary[i] == '0') ? '1' : '0';

    return TRUE;
}

/*a helper function to add 1 to the the binary repressentation for negative numbers. Part of the 2's complement steps*/
int add_one(char binary[]){
    int carry, i;
    carry = 1;
    i = strlen(binary) - 1;

    /* adding one */
    while (carry == 1 && i >= 0){
        if (binary[i] == '0'){
            binary[i] = '1';
            carry = 0;
        }
        else
            binary[i] = '0';
        i--;
    }
    return TRUE;
}

int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int isData, int lineNum){
    (*labelTable) = (label *)realloc((*labelTable), sizeof(label) * ((*labelCount) + 1));

    if (find_label(*labelTable, labelName, *labelCount) != NULL)
        return fp_error_handler(Label_Already_Defined, lineNum);

    strcpy((*labelTable)[*labelCount].labelName, labelName);
    (*labelTable)[*labelCount].line = lineNum;
    (*labelTable)[*labelCount].address = counterValue + 100; /*change that to a pointer outside of the function*/
    (*labelTable)[*labelCount].isData = isData;
    (*labelCount)++;
    return 0;
}

/* might remove this function */
int add_are(char *binary, char *are){
    strcat(binary, "00");
    return 0;
}

int add_first_op_word(char ***icImage, operation *op, int *ic){
    char binary[13]; /* 12 binary digits + 1 null terminator */

    memset(binary, '\0', 13);

    convert_to_binary(binary + 0, op->sourceOperand->addrMode, 3);
    convert_to_binary(binary + 3, op->opcode, 4);
    convert_to_binary(binary + 7, op->destOperand->addrMode, 3);
    add_are(binary, ABSOLUTE);

    add_to_counterArray(icImage, ic, binary);

    return 0;
}

int is_operation(char *opname){
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

    return -1;
}

int analyze_string(char* stringLine, int* stringConverted, int lineNum){
    int i;

    if (stringLine[0] != '\"' || stringLine[strlen(stringLine) - 1] != '\"')
        return fp_error_handler(Illegal_String_Declaration, lineNum);

    stringLine++;
    stringLine[-1] = '\0'; /* this line might cause problem: i need to check if it wont cause problem and if its even ok to do like that. */
    stringLine[strlen(stringLine) - 1] = '\0';

    for (i = 0; i < strlen(stringLine); i++){
        stringConverted[i] = stringLine[i];
    }
    stringConverted[i] = 0;

    return i + 1;
    /* write_data(stringConverted, strlen(stringLine)+1, lineNum); */
}

int analyze_data(char** token, int *params, int lineNum){
    int i;

    i = 0;
    while (*token != NULL){
        if (**token != ','){
            if (strspn((*token), "-+0123456789") == strlen((*token))){
                params[i] = atoi((*token));
            }
            else
                return fp_error_handler(Invalid_Parameter_Data, lineNum); /* input isnt a number */
        }
        (*token) = strtok(NULL, PARAM_DELIMITERS);
        i++;
    }

    return TRUE;
}

int add_data_word(char ***dcImage, int *params, int *dc, int paramCnt, int lineNum){
    int i;
    char binary[13];

    for (i = 0; i < paramCnt; i++){
        convert_to_binary(binary, params[i], 12);
        add_to_counterArray(dcImage, dc, binary);
    }

    return TRUE;
}

int get_extent_label_type(extentlabel* head, char* labelName){
    while (head != NULL){
        if (strcmp(head->labelName, labelName) == 0)
            return head->type;
        head = head->next;
    }

    return FALSE;
}

/* want to change implementation */
int add_extent_label(extentlabel** head, char** token, int type, int lineNum){
    extentlabel* newNode;
    extentlabel* temp;
    int changemename;
    newNode = (extentlabel *)calloc(1, sizeof(extentlabel));

    changemename = get_extent_label_type(*head, *token);
    if (changemename != FALSE){
        if (changemename == type)
            fp_warning_handler(Extent_Label_Already_Defined_Similarly, lineNum);
        else
            return fp_error_handler(Extent_Label_Already_Defined_Differently, lineNum);
    }
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

    return TRUE;
}

int is_label(char **token, char *labelName, char *line, int lineNum){
    if (strlen(*token)-1 > MAX_LABEL_LENGTH)
        return fp_error_handler(Label_Name_Length_Too_Long, lineNum);
    
    if (LAST_CHARACTER(*token) != ':')
        return FALSE;
    else{
        LAST_CHARACTER(*token) = '\0';
        strcpy(labelName, *token);
        *token = strtok(NULL, PARAM_DELIMITERS);
    }
    if (*token == NULL)
        return fp_error_handler(Blank_Label_Declaration, lineNum);

    if (is_extent_instruction(labelName) != FALSE || is_operation(labelName) != -1 || is_datastring_instruction(labelName) != FALSE)
        return fp_error_handler(Illegal_Comma_Name_Saved_Word, lineNum);
    
    if (!(65 <= *labelName && *labelName <= 90) && !(97 <= *labelName && *labelName <= 122))
        return fp_error_handler(Illegal_Comma_Name_First_Char, lineNum);

    if (strspn(labelName, "012345678abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != strlen(labelName))
        return fp_error_handler(Illegal_Comma_Name_Illegal_Chars, lineNum);
    

    return TRUE;
}

label* find_label(label *labelTable, char *labelName, int labelCount){
    int i;

    for (i = 0; i < labelCount; i++)
        if (strcmp(labelTable[i].labelName, labelName) == 0)
            return labelTable+i; /* need to return the address plus 100 */

    return NULL;
}

/* adds the operand words to the operation array */
int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount){
    int sourceVal, destVal;
    int sourceStatus, destStatus;
    char binary[31];

    sourceStatus = get_type_val(labelTable, op->sourceOperand, &sourceVal, labelCount);
    destStatus = get_type_val(labelTable, op->destOperand, &destVal, labelCount);

    if (BOTH_ARE_REGISTER(op)){
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

int get_operand_value(operand *op, char *token, int lineNum){
    char foo[1];

    token = strtok(NULL, PARAM_DELIMITERS);
    if (token == NULL){
        op->val.numericVal = atoi(token);
        op->addrMode = No_Operand;
    }
    /* not sure if can input + aswell, check that */
    else if (strspn(token, "+-0123456789") == strlen(token)){
        op->val.numericVal = atoi(token);
        op->addrMode = Immediate;
    }
    else if (token[0] == '@' && token[1] == 'r'){
        foo[0] = token[3];
        if (strspn(foo, "0123456789") == 1)
            return fp_error_handler(Undefined_Register, lineNum);
        foo[0] = token[2];
        if (strspn(foo, "01234567") == 1){
            op->val.regNum = atoi(foo);
            op->addrMode = Register;
        }
        else
            return fp_error_handler(Undefined_Register, lineNum);
        /* return error */
    }
    else{
        strcpy(op->val.labelName, token);
        op->addrMode = Direct;
    }

    return TRUE;
}

/* GO OVER THIS AND REWRITE THE WHOLE MECHANISM THIS FUNCITON IS PART OF */
int get_type_val(label **labelTable, operand *operandd, int *val, int labelCount){
    label* temp;

    switch (operandd->addrMode){
    case Direct:
        temp = find_label(*labelTable, operandd->val.labelName, labelCount);
        if (temp == NULL)
            *val = 0;
        else
            *val = temp->address;
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
    memset(binary, '\0', 31);

    convert_to_binary(binary, sourceVal, 5);
    convert_to_binary(binary + 5, destVal, 5);
    binary[10] = '0'; /* might not work sometime maybe? need to check this */
    binary[11] = '0';

    return TRUE;
}

int get_isolated_word(operand *operandd, char* binary, int val, int status){
    if (status == FALSE || operandd->addrMode == No_Operand)
        return FALSE;

    memset(binary, '\0', 13);
    if (operandd->addrMode == Register)
        get_register_word(binary, val, 0);
    else if (operandd->addrMode == Direct)
        strcpy(binary, operandd->val.labelName); /* binary size is too small */
    else{
        convert_to_binary(binary, val, 10);
        binary[10] = '0'; /* keep getting segmentation fault here sometime! */
        binary[11] = '0';
    }

    return TRUE;
}

int set_operands(operation *op, operand *operand1, operand *operand2){
    if ((operand1->addrMode == 0 && operand2->addrMode == 0) || (operand1->addrMode != 0 && operand2->addrMode != 0)){
        op->sourceOperand = operand1;
        op->destOperand = operand2;
    }
    else{
        op->sourceOperand = operand2;
        op->destOperand = operand1;
    }

    return TRUE;
}

int get_comma_param_cnt(char* line, int lineNum){
    int i, status1, status2, paramCnt, commaCnt;

    i = paramCnt = commaCnt = status1 = status2 = FALSE;
    if (line == NULL)
        return TRUE;

    while (line[i] != '\0'){
        if (line[i] == ','){
            status2 = status1;
            status1 = _COMMA;
            commaCnt++; 
        }
        else if (line[i] != ' ' && line[i] != '\t'){
            status2 = status1;
            status1 = _CHAR;
            paramCnt++; 
            while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0' && line[i] != ',')
                i++;
            i--;
        }
        if (status1 == status2 && status1 != FALSE){
            if (status1 == _COMMA)
                return fp_error_handler(Double_Comma, lineNum);   /* two commas in a row */
            else if (status1 == _CHAR)
                return fp_error_handler(Missing_Comma, lineNum);   /* two parameters in a row */
        }
        i++;
        if (commaCnt > paramCnt)
            return fp_error_handler(Extra_Comma, lineNum);
    }
    if (paramCnt == commaCnt)
        return fp_error_handler(Extra_Comma, lineNum);   /* extra comma at the end or at the start */
    return paramCnt;
}

int is_extent_instruction(char* token){
    if (strcmp(token, ".entry") == 0)
        return _ENTRY;
    if (strcmp(token, ".extern") == 0)
        return _EXTERN;

    return FALSE;
}

int is_datastring_instruction(char* token){
    if (strcmp(token, ".data") == 0)
        return _DATA;
    if (strcmp(token, ".string") == 0)
        return _STRING;

    return FALSE;
}

int call_datastring_analyzer(char** lineToken, int** params, char* orgLineToken, int lineNum, int commandCode){    
    *params = (int*)calloc(1, sizeof(int));   /* move this to a different function */
    
    *lineToken = strtok(NULL, PARAM_DELIMITERS);

    if (commandCode == _DATA){
        analyze_data(lineToken, *params, lineNum);
        return get_comma_param_cnt(orgLineToken, lineNum);
    }
    
    return analyze_string(*lineToken, *params, lineNum);
}

  /* get the pointer to the first parameter in the line */
  /* fix this function */
char* get_param_pointer(char* orgLineToken, char toFind){
    int i;
    i = 0;

    while (orgLineToken[i+1] != '\n' && orgLineToken[i+1] != '\0'){
        if (orgLineToken[i] == toFind && (orgLineToken[i+1] == ' ' || orgLineToken[i+1] == '\t'))
            return orgLineToken + i+1;
        i++; 
    }

    return NULL;
}

int set_sequence_array_dest(char sequenceArray[16][4]){
    strcpy(sequenceArray[Mov], " 35\0");
    strcpy(sequenceArray[Cmp], "135\0");
    strcpy(sequenceArray[Add], " 35\0");
    strcpy(sequenceArray[Sub], " 35\0");
    strcpy(sequenceArray[Not], " 35\0");
    strcpy(sequenceArray[Clr], " 35\0");
    strcpy(sequenceArray[Lea], " 35\0");
    strcpy(sequenceArray[Inc], " 35\0");
    strcpy(sequenceArray[Dec], " 35\0");
    strcpy(sequenceArray[Jmp], " 35\0");
    strcpy(sequenceArray[Bne], " 35\0");
    strcpy(sequenceArray[Red], " 35\0");
    strcpy(sequenceArray[Prn], "135\0");
    strcpy(sequenceArray[Jsr], " 35\0");
    strcpy(sequenceArray[Rts], "0  \0");
    strcpy(sequenceArray[15], "0  \0");

    return TRUE;
}

int set_sequence_array_source(char sequenceArray[16][4]){
    strcpy(sequenceArray[Mov], "135\0");
    strcpy(sequenceArray[Cmp], "135\0");
    strcpy(sequenceArray[Add], "135\0");
    strcpy(sequenceArray[Sub], "135\0");
    strcpy(sequenceArray[Not], "0  \0");
    strcpy(sequenceArray[Clr], "0  \0");
    strcpy(sequenceArray[Lea], "3  \0");
    strcpy(sequenceArray[Inc], "0  \0");
    strcpy(sequenceArray[Dec], "0  \0");
    strcpy(sequenceArray[Jmp], "0  \0");
    strcpy(sequenceArray[Bne], "0  \0");
    strcpy(sequenceArray[Red], "0  \0");
    strcpy(sequenceArray[Prn], "0  \0");
    strcpy(sequenceArray[Jsr], "0  \0");
    strcpy(sequenceArray[Rts], "0  \0");
    strcpy(sequenceArray[Stop], "0  \0");

    return TRUE;
}
 
  /* try cleaning up and bit and making this functions more readable and more focused */
int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount){
    FILE* amFile;
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    char labelName[MAX_LABEL_LENGTH];
    char sourceSequenceArray[16][4], destSequenceArray[16][4];
    int commandCode, lineNum, isLabel, counterVal, isData;

    *dc = *ic = *labelCount = lineNum = 0;
    /* move this to assembler program, maybe make this global variable */
    set_sequence_array_source(sourceSequenceArray);
    set_sequence_array_dest(destSequenceArray);

    while (read_input_file(&amFile, filename, ".am", originalLine, &lineNum) == TRUE){
        strcpy(line, originalLine);
        token = strtok(line, PARAM_DELIMITERS);

        isLabel = is_label(&token, labelName, line, lineNum);   /* NOTE: if label declaration is emtpy we get segmantation fault. */

        if ((commandCode = is_extent_instruction(token)) != FALSE){
            counterVal = *dc;
            isData = TRUE;

            token = strtok(NULL, PARAM_DELIMITERS);
            add_extent_label(head, &token, commandCode, lineNum);
        }
        else if ((commandCode = is_datastring_instruction(token)) != FALSE){
            int* params;
            int paramCnt;

            counterVal = *dc;
            isData = TRUE;

            paramCnt = call_datastring_analyzer(&token, &params, get_param_pointer(originalLine, LAST_CHARACTER(token)), lineNum, commandCode);
            if (paramCnt != -1)
                add_data_word(dcImage, params, dc, paramCnt, lineNum);
        }
        else if ((commandCode = is_operation(token)) != -1){
            counterVal = *ic;
            isData = FALSE;

            if (get_comma_param_cnt(get_param_pointer(originalLine, LAST_CHARACTER(token)), lineNum) != -1){
                operation op;
                operand operand1, operand2;

                op.opcode = commandCode;

                if (get_operand_value(&operand1, token, lineNum) != ERROR && get_operand_value(&operand2, token, lineNum) != ERROR){
                    set_operands(&op, &operand1, &operand2);   /* match operand1 and  operand2 to the correct source and dest */

                    if (check_param_sequence(sourceSequenceArray, op.sourceOperand, op.opcode, lineNum, _SOURCE) == TRUE && check_param_sequence(destSequenceArray, op.destOperand, op.opcode, lineNum, _DEST) == TRUE){
                        add_first_op_word(icImage, &op, ic);                            /* adding the first word of the operation */
                        add_operand_words(icImage, labelTable, &op, ic, *labelCount);   /* adding the operation words */
                    }
                }
            }
        }
        else
            fp_error_handler(Unknown_Command, lineNum);

        if (isLabel == TRUE){
            if (commandCode == _ENTRY || commandCode == _EXTERN)
                fp_warning_handler(Label_Points_At_ExternEntry, lineNum);
            else
                add_label(labelTable, labelName, labelCount, counterVal, isData, lineNum); // remove ic and dc, make them global variables
              /* NOTE: if label there is not command, and then counter isnt pointing anywhere valid, what do we do? */
        }
    }

      /* close files */
      /* free memory */
    return TRUE;
}

int check_param_sequence(char sequenceArr[16][4], operand* operandd, int opcode, int lineNum, int sourceOrDest){
    char addrMode;
    int j;

    addrMode = operandd->addrMode + '0';
    j = 0;

    while (sequenceArr[opcode][j] != '\0' && sequenceArr[opcode][j] != addrMode)
        j++;
    
    if (sequenceArr[opcode][j] == '\0'){
        if (sourceOrDest == _SOURCE)
            return fp_error_handler(Invalid_Source_Sequence, lineNum);
        else
            return fp_error_handler(Invalid_Dest_Sequence, lineNum);
    }
    return TRUE;
}

int fp_warning_handler(int warningCode, int lineNum){
    switch (warningCode){
    case Label_Points_At_ExternEntry:
        printf("Warning: Label points at .extern/.entry. Assembler will ignore the label. Line %d\n", lineNum);
        break;
    case Extent_Label_Already_Defined_Similarly:
        printf("Warning: Extent label already defined with same type. Line %d\n", lineNum);
        break;
    }

    return TRUE;
}

int fp_error_handler(int errorCode, int lineNum){
    switch (errorCode){
    case Unknown_Command:
        printf("Error: Unknown command in line %d\n", lineNum);
        break;
    case Comma_Error:
        printf("Error: Comma error in line %d\n", lineNum);
        break;
    case Extra_Comma:
        printf("Error: Extra comma at the end/start in line %d\n", lineNum);
        break;
    case Double_Comma:
        printf("Error: Double comma in line %d\n", lineNum);
        break;
    case Missing_Comma:
        printf("Error: Missing comma. Line %d\n", lineNum);
        break;
    case Invalid_Parameter_Data:
        printf("Error: Non whole numerical value found in '.data'. Line %d\n", lineNum);
        break;
    case Invalid_Source_Sequence:
        printf("Error: Invalid source operand sequence. Line %d\n", lineNum);
        break;
    case Invalid_Dest_Sequence:
        printf("Error: Invalid destination operand sequence. Line %d\n", lineNum);
        break;
    case Illegal_String_Declaration:
        printf("Error: Illegal string declaration (probably \"\"). Line %d\n", lineNum);
        break;
    case Label_Already_Defined:
        printf("Error: Label already defined beforehand. Line %d\n", lineNum);
        break;
    case Illegal_Comma_Name_Saved_Word:
        printf("Error: Illegal label name. Label cant be named a saved word Line %d\n", lineNum);
        break;    
    case Illegal_Comma_Name_First_Char:
        printf("Error: Illegal label name. First character must be a letter. Line %d\n", lineNum);
        break;
    case Illegal_Comma_Name_Illegal_Chars:
        printf("Error: Illegal label name. Label name can only contain letters and numbers. Line %d\n", lineNum);
        break;
    case Extent_Label_Already_Defined_Differently:
        printf("Error: Extent label already defined a different type. Line %d\n", lineNum);
        break;
    case Label_Name_Length_Too_Long:
        printf("Error: Label name exceeds max length. Line %d\n", lineNum);
        break;
    case Undefined_Register:
        printf("Error: Undefined register. Line %d\n", lineNum);
        break;
    case Blank_Label_Declaration:
        printf("Error: No code found after label declaration. Line %d\n", lineNum);
        break;
    }

    return ERROR;
}

/** QUESTIONS: 
 * 1. if encountered illegal label name, should we only ignore the label or ignore the whole line?
*/

/** NOTES:
 * 1. change all atoi uses into strtol or something like that
 * 2. Need to read and understand ARE.
 * 3. maybe instead of analyzing and reporting errors from each function, create a sub function to be called, and that 
 *   function's purpose is to check if there is an error, and call fp_error_handler accordingly. This could maybe help 
 *   with keeping the code more modular and functions shorter.
 * 4. Need to remove structs from the header file.
*/
