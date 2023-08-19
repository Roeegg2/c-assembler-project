#include "firstpass.h"
#include "shared.h"
#include "utils.h"


int fp_status; /*A flag variable. Its value is error if one is found, otherwise it stays true*/
int fp_lineNum; /*A variable to keep track of the current line of the .am file*/

int invoke_firstpass(char*** dcImage, char*** icImage, label** labelTable, extentlabel** head, char* filename, int* dc, int* ic, int* labelCount, char sourceSequenceArray[16][4], char destSequenceArray[16][4]){
    FILE* amFile;
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH]; 
    char labelName[MAX_LABEL_MACRO_LENGTH];
    int commandCode, isLabel, counterVal, isInstruction;

    fp_status = TRUE;
    *labelCount = fp_lineNum = 0;

    /*Reading input from file*/
    while (read_input_file(&amFile, filename, ".am", originalLine, &fp_lineNum) == TRUE){
        strcpy(line, originalLine);
        token = strtok(line, " \t\n");

        /*Check if we got label declaration, save the result*/
        isLabel = is_label(&token, labelName, line);

        /*Setting counter and isInstruction flag to data by default */
        counterVal = *dc;
        isInstruction = TRUE;

        if (isLabel != ERROR){ /*Avoiding invalid label declaration*/
            if ((commandCode = is_extent_instruction(token)) != FALSE) /*If we got .ext/.ent instruction*/
                extent_handler(head, &token, originalLine, commandCode);

            else if ((commandCode = is_datastring_instruction(token)) != FALSE) /*If we got .data/.string instruction*/
                datastring_handler(dcImage, &token, originalLine, dc, commandCode);

            else if ((commandCode = is_operation(token)) != -1){
                /*Changing the default values from data*/
                counterVal = *ic;
                isInstruction = FALSE;

                operation_handler(icImage, labelTable, &token, originalLine, ic, labelCount, commandCode, sourceSequenceArray, destSequenceArray);
            }
            else /*if its not any of the commands above, its an invalid command*/
                fp_error_handler(Unknown_Command);

            if (isLabel == TRUE){
                if (commandCode == _ENTRY || commandCode == _EXTERN) /*If we get label declaration on .ext/.ent, we report error*/
                    fp_warning_handler(Label_Points_At_ExternEntry);
                else
                    add_label(labelTable, labelName, labelCount, counterVal, isInstruction);  /*Otherwise, add the label*/
            }
        }
    }

    CHECK_BUFFER_OVERFLOW((*dc), (*ic)) /*Making sure we dont exceed machine memory (924) */

    fclose(amFile);
    return fp_status;
}

int check_param_sequence(char sequenceArr[16][4], operand* operandd, int opcode, int sourceOrDest){
    char addrMode;
    int j;

    addrMode = operandd->addrMode + '0';
    j = 0;

    /*Check if the addr is valid for this operation*/
    while (sequenceArr[opcode][j] != '\0' && sequenceArr[opcode][j] != addrMode)
        j++;
    
    /*If we reached the end of the string - meaning the operation addrMode isnt*/
    if (sequenceArr[opcode][j] == '\0'){
        if (sourceOrDest == _SOURCE){
            if (addrMode == '0') /*If we have no operand and, no operand is invalid*/
                return fp_error_handler(Missing_Source_Operand);
            return fp_error_handler(Invalid_Source_Sequence); /*If we have an operand, but that operand is invalid*/
        }
        else{ /*Same thing but with _DEST*/
            if (addrMode == '0') 
                return fp_error_handler(Missing_Dest_Operand);
            return fp_error_handler(Invalid_Dest_Sequence);
        }
    }
    return TRUE;
}


/*function that receives a string, a number and a size, and writes into the string the binary */
/* what is counter type? maybe switch (*labelTable) with something more readable? */
/* might break when number entered is negative */
int convert_to_binary(char binary[], int number, int size){
    int isNegative, i;

    memset(binary, '0', size);
    binary[size] = '\0';

    /*Get numbers abs value*/
    isNegative = (number < 0) ? 1 : 0;
    if (isNegative == 1)
        number *= -1;

    /*Getting binary encoding of abs value*/
    i = size - 1;
    while (number > 0){
        binary[i] = '0' + number % 2;
        number /= 2;
        i--;
    }

    /*Additional operations of number is negative*/
    if (isNegative == TRUE){
        flip_negative(binary);
        add_one(binary);
    }
    return 0;
}

int flip_negative(char binary[]){
    int i;

    for (i = 0; i < strlen(binary); i++)
        binary[i] = (binary[i] == '0') ? '1' : '0';

    return TRUE;
}

/*a helper function to add 1 to the the binary repressentation for negative numbers. Part of the 2's complement steps*/
int add_one(char binary[]){
    int carry, i;
    carry = 1;

    i = strlen(binary) - 1;
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

int add_label(label **labelTable, char *labelName, int *labelCount, int counterValue, int isInstruction){
    (*labelTable) = (label *)realloc((*labelTable), sizeof(label) * ((*labelCount) + 1));
    CHECK_ALLOCATION_ERROR((*labelTable))

    /*If a label with the same name was already declared*/
    if (find_label(*labelTable, labelName, *labelCount) != NULL)
        return fp_error_handler(Label_Already_Defined);

    strcpy((*labelTable)[*labelCount].labelName, labelName);
    (*labelTable)[*labelCount].line = fp_lineNum;
    (*labelTable)[*labelCount].address = counterValue + 100; /*Adding the counter value + the program start memory- 100*/
    (*labelTable)[*labelCount].isInstruction = isInstruction;
    (*labelCount)++;

    return 0;
}

int add_first_op_word(char ***icImage, operation *op, int *ic){
    char binary[13]; /* 12 binary digits + 1 null terminator */

    convert_to_binary(binary + 0, op->sourceOperand->addrMode, 3); /*Getting source addressing mode encoding*/
    convert_to_binary(binary + 3, op->opcode, 4); /*Endcoding of the opcode*/
    convert_to_binary(binary + 7, op->destOperand->addrMode, 3); /*Getting destination addressing mode encoding*/
    if (op->sourceOperand->addrMode == Direct || op->destOperand->addrMode == Direct)
        binary[10] = binary[11] = '2';  /*Adding ARE to be changed later (in secondpass, after knowing what label type is)*/
    else
        binary[10] = binary[11] = '0'; /*If both operands arent direct, we can set ARE to absolute*/
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

int check_string_formatting(char* stringLine, int *index1, int *index2){
    int i;

    *index1 = *index2 = FALSE;
    i = 0;

    while (i < strlen(stringLine) && *index1 == FALSE){
        if (stringLine[i] == '\"') /*If we found " */
            *index1 = i;
        else if (stringLine[i] != '\t' && stringLine[i] != ' ')/* If we havent found a " and its not a whitenote - that means there is a character outside of "" - error*/
            return FALSE;
        i++;
    }

    i = strlen(stringLine)-1;
    while (i > 0 && *index2 == FALSE){ 
        /* If we found " and it isnt the one we found earlier*/
        if (stringLine[i] == '\"' && *index1 != i)
            *index2 = i;
        /* If the current char isnt a whitenote - meaning there is text outside of "" */
        else if (stringLine[i] != '\t' && stringLine[i] != ' ' && stringLine[i] != '\n')
            return FALSE;
        i--;
    }
    /* if we havent found 2 "" */
    if (*index1 == FALSE || *index2 == FALSE)
        return FALSE;
    
    return TRUE;
}

int analyze_string(int** stringConverted, char* stringLine){
    int index1, index2;
    int i;

    if (check_string_formatting(stringLine, &index1, &index2) == FALSE)
        return fp_error_handler(Illegal_String_Declaration);

    /* I dont really like the way were doing it here */
    stringLine[index1] = '\0';
    stringLine[index2] = '\0';
    stringLine += index1+1;

    for (i = 0; i < strlen(stringLine); i++){
        (*stringConverted) = (int*)realloc((*stringConverted), sizeof(int) * (i + 1));
        CHECK_ALLOCATION_ERROR((*stringConverted))

        (*stringConverted)[i] = (int)stringLine[i];
        if ((*stringConverted)[i] < 32 || (*stringConverted)[i] > 126)
            return fp_error_handler(Illegal_String_Char);
    }
    /*adding the null terminator word */
    (*stringConverted) = (int*)realloc((*stringConverted), sizeof(int) * (i + 1));
    CHECK_ALLOCATION_ERROR((*stringConverted))

    (*stringConverted)[i] = 0;

    return i+1;
    /* write_data(stringConverted, strlen(stringLine)+1); */
}

int analyze_data(char** token, int** params){
    char* endptr;
    int i;

    i = 0;
    while (*token != NULL){
        if (**token != ','){
            if (strspn((*token), "-+0123456789") == strlen((*token))){ /*If we have indeed a valid parameter (whole numbers only)*/
                (*params) = (int*)realloc((*params), sizeof(int) * (i + 1));
                CHECK_ALLOCATION_ERROR((*params))

                (*params)[i] = strtol(*token, &endptr, 10);
                if ((*params)[i] < -2048 || (*params)[i] > 2047) /*Making sure parameter isnt out of bounds*/
                    return fp_error_handler(Parameter_Out_Of_Bounds);
            }
            else
                return fp_error_handler(Parameter_Not_Whole_Number); /* Input isnt a number */
        }
        (*token) = strtok(NULL, PARAM_DELIMITERS);
        i++;
    }

    return TRUE;
}

int add_data_word(char ***dcImage, int *params, int *dc, int paramCnt){
    int i;
    char binary[13];

    for (i = 0; i < paramCnt; i++){ 
        convert_to_binary(binary, params[i], 12); /*Convert each param to binary*/
        add_to_counterArray(dcImage, dc, binary); /*Add it to the ic array*/
    }

    return TRUE;
}

extentlabel* find_extent_label(extentlabel* head, char* labelName){
    while (head != NULL){
        if (strcmp(head->labelName, labelName) == 0)
            return head; /*Get the address of the node we are looking for*/
        head = head->next;
    }

    return NULL;
}

/* want to change implementation */
int add_extent_label(extentlabel** head, char** token, int type){
    extentlabel* newNode;
    extentlabel* temp;

    newNode = (extentlabel *)calloc(1, sizeof(extentlabel));
    CHECK_ALLOCATION_ERROR((newNode))

    /*checking and making sure that the label hasnt been declared yet. Maybe move to a differnt function?*/
    temp = find_extent_label(*head, *token);
    if (temp != NULL){
        if (temp->type == type)
            fp_warning_handler(Extent_Label_Already_Defined_Similarly);
        else
            return fp_error_handler(Extent_Label_Already_Defined_Differently);
    }

    /*If first node to add*/
    if ((*head) == NULL)
        (*head) = newNode;

    temp = (*head);
    while (temp->next != NULL) /*Getting the last node*/
        temp = temp->next;

    if (temp != newNode) /*If we dont have only 1 node */
        temp->next = newNode;

    strcpy(newNode->labelName, *token);
    newNode->type = type;
    newNode->line = fp_lineNum;
    newNode->next = NULL;

    return TRUE;
}

int is_label(char **token, char *labelName, char *line){    
    if (LAST_CHARACTER(*token) != ':') /*If token doesnt end with ':' - it is not a label declaration*/
        return FALSE;
    else{ /*Removing the ':' to extract labelName*/
        LAST_CHARACTER(*token) = '\0';
        strcpy(labelName, *token);
        *token = strtok(NULL, PARAM_DELIMITERS);
    }
    if (*token == NULL)
        return fp_error_handler(Blank_Label_Declaration);

    return legal_label_macro_name(labelName, &fp_error_handler); /* Some more checking for errors*/   
}

label* find_label(label *labelTable, char *labelName, int labelCount){
    int i;

    for (i = 0; i < labelCount; i++)
        if (strcmp(labelTable[i].labelName, labelName) == 0) /* If the name prompted and the name of current label match*/
            return labelTable+i; /* Returning the labelTable pointer + the label we want position (memory address of labelTable + the position we want) */

    return NULL;
}

int add_operand_words(char ***icImage, label **labelTable, operation *op, int *ic, int labelCount){
    int sourceVal, destVal;
    char binary[MAX_LABEL_MACRO_LENGTH+10];

    /* Getting the value we need to encode, depending on the type of the operand. The status are indicaters they are indeed valid operands*/
    get_type_val(labelTable, op->sourceOperand, &sourceVal, labelCount);
    get_type_val(labelTable, op->destOperand, &destVal, labelCount);

    /* If both operands are registers, we have 1 combined word*/
    if (BOTH_ARE_REGISTER(op)){
        get_register_word(binary, sourceVal, destVal);
        add_to_counterArray(icImage, ic, binary);
    }
    else{ /*If both operands arent register, we have 1 word for each operand*/
        if (get_isolated_word(op->sourceOperand, binary, sourceVal) == TRUE)
            add_to_counterArray(icImage, ic, binary);
            
        if (get_isolated_word(op->destOperand, binary, destVal) == TRUE)
            add_to_counterArray(icImage, ic, binary);
    }

    return TRUE;
}

int get_operand_value(operand* op, char* token){
    char foo[1];
    char* endptr;

    token = strtok(NULL, PARAM_DELIMITERS);
    if (token == NULL){ /*If we have no operand*/
        op->val.numericVal = 0;
        op->addrMode = No_Operand;
    }
    else if (strspn(token, "+-0123456789") == strlen(token)){ /*If all characters are numbers - we have numeric param*/
        op->val.numericVal = strtol(token, &endptr, 10);
        op->addrMode = Immediate;
        if (op->val.numericVal > 511 || op->val.numericVal < -512) /*Checking range*/
            return fp_error_handler(Parameter_Out_Of_Bounds);
    }
    else if (token[0] == '@' && token[1] == 'r'){ /*If param starts with @r*/
        foo[0] = token[3];
        if (strspn(foo, "0123456789") == 1) /*If the numebr is 2 digits - invalid reg num*/
            return fp_error_handler(Undefined_Register);

        foo[0] = token[2];
        if (strspn(foo, "01234567") == 1){ /*Even if reg num is 1 digit, it still could be 8 or 9 which is an error*/
            op->val.regNum = strtol(foo, &endptr, 10);
            op->addrMode = Register;
        }
        else
            return fp_error_handler(Undefined_Register);
        /* return error */
    }
    else{ /*If its not anything else, its a label*/
        strcpy(op->val.labelName, token);
        op->addrMode = Direct;
    }

    return TRUE;
}
/* Function to determine the value of an operand based on its addressing mode */
int get_type_val(label **labelTable, operand *operandd, int *val, int labelCount) {
    label* temp;

    switch (operandd->addrMode) {
        case Direct:
            temp = find_label(*labelTable, operandd->val.labelName, labelCount);
            if (temp == NULL)
                *val = 0;
            else
                *val = temp->address; /* Assign the address of the label */
            break;
        case Immediate:
            *val = operandd->val.numericVal; /* Assign the immediate numeric value */
            break;
        case Register:
            *val = operandd->val.regNum; /* Assign the register number */
            break;
    }

    return TRUE;
}

int get_register_word(char* binary, int sourceVal, int destVal) {
    memset(binary, '\0', 31); /* Clear the binary buffer */

    /* Convert sourceVal and destVal to binary and store them in the buffer, 5 bits space for each */
    convert_to_binary(binary, sourceVal, 5);
    convert_to_binary(binary + 5, destVal, 5);
    binary[10] = '0'; /* Setting ARE to absolute */
    binary[11] = '0';

    return TRUE;
}

int get_isolated_word(operand *operandd, char* binary, int val) {
    if (operandd->addrMode == No_Operand)
        return FALSE;

    memset(binary, '\0', 13); /* Clear the binary buffer */

    if (operandd->addrMode == Register)
        get_register_word(binary, val, 0); /* If we have one reg operand  */
    else if (operandd->addrMode == Direct)
        strcpy(binary, operandd->val.labelName); /* Copy the label name to the buffer, to be analyzed later in secondpass */ 
    else {
        convert_to_binary(binary, val, 10); /* Convert val to binary of 10 bits*/
        binary[10] = '0'; /* Setting ARE for the last 2 bits */
        binary[11] = '0';
    }

    return TRUE;
}

int set_operands(operation *op, operand *operand1, operand *operand2) {
    /* If there are 0 or 2 operands*/
    if ((operand1->addrMode == 0 && operand2->addrMode == 0) || (operand1->addrMode != 0 && operand2->addrMode != 0)) {
        op->sourceOperand = operand1;
        op->destOperand = operand2;
    } 
    else { /* If we have 1 operand*/
        op->sourceOperand = operand2;
        op->destOperand = operand1;
    }

    return TRUE;
}

int get_comma_param_cnt(char* line) {
    int i, status1, status2, paramCnt, commaCnt;

    i = paramCnt = commaCnt = status1 = status2 = FALSE;
    if (line == NULL)
        return TRUE;

    /*While we havent reached the end of the string*/
    while (line[i] != '\n' && line[i] != '\0') {
        if (line[i] == ',') {
            status2 = status1;
            status1 = _COMMA;
            commaCnt++; 
        } else if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0') {  /* If we found parameter */
            status2 = status1;
            status1 = _CHAR;
            paramCnt++; 
            /* Continuing the loop, for parameters which consist odf multiple chars (registers for example) */
            while (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && line[i] != '\0' && line[i] != ',')
                i++;
            i--;
        }
        /* If found 2 symbols in a row (either params or commas)*/
        if (status1 == status2 && status1 != FALSE) {
            if (status1 == _COMMA)
                return fp_error_handler(Double_Comma); /* Two commas in a row */
            else if (status1 == _CHAR)
                return fp_error_handler(Missing_Comma); /* Two parameters in a row */
        }
        i++;
        if (commaCnt > paramCnt)
            return fp_error_handler(Extra_Comma);
    }
    if (paramCnt == commaCnt)
        return fp_error_handler(Extra_Comma); /* Extra comma at the end or at the start */

    return paramCnt;
}

int is_extent_instruction(char* token) {
    if (strcmp(token, ".entry") == 0)
        return _ENTRY;
    if (strcmp(token, ".extern") == 0)
        return _EXTERN;

    return FALSE;
}

int is_datastring_instruction(char* token) {
    if (strcmp(token, ".data") == 0)
        return _DATA;
    if (strcmp(token, ".string") == 0)
        return _STRING;

    return FALSE;
}

int call_datastring_analyzer(char** lineToken, int** params, char* orgLineToken, int commandCode) {    
    *params = (int*)calloc(1, sizeof(int));   /* Allocate memory for params */
    CHECK_ALLOCATION_ERROR((*params))

    *lineToken = strtok(NULL, PARAM_DELIMITERS);
    if (*lineToken == NULL)
        return fp_error_handler(Blank_DataString_Instruction);

    if (commandCode == _DATA) {
        analyze_data(lineToken, params); /*If command is Data analyze data parameters */
        return get_comma_param_cnt(orgLineToken);
    }

    return analyze_string(params, orgLineToken); /* Analyze string parameters */
}

char* get_param_pointer(char* orgLineToken, char toFind) {
    int i;
    i = 0;

    /* While we havent reached the end of the string */
    while (orgLineToken[i+1] != '\n' && orgLineToken[i+1] != '\0') {
        /* If we found the start of parameters */
        if (orgLineToken[i] == toFind && (orgLineToken[i+1] == ' ' || orgLineToken[i+1] == '\t' || orgLineToken[i+1] == ',')) {
            return orgLineToken + i+1; /* Return pointer to the next parameter */
        }
        i++; 
    }

    return NULL; /* No more parameters found */
}

int set_sequence_array_dest(char sequenceArray[16][4]) {
    /* Initialize sequenceArray with destination operand sequences for various opcodes */
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

    return TRUE; /* Array initialization successful */
}

int set_sequence_array_source(char sequenceArray[16][4]) {
    /* Initialize sequenceArray with source operand sequences for various opcodes */
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

    return TRUE; /* Array initialization successful */
}

int extent_handler(extentlabel** head, char** token, char* originalLine, int commandCode) {
    char* paramsPtr;

    /* Find the pointer to the parameters after the token */
    paramsPtr = get_param_pointer(originalLine, LAST_CHARACTER(*token));
    
    *token = strtok(NULL, " \t\n"); /* Move to the next token */
    if (*token == NULL)
        return fp_error_handler(Blank_Extent_Marking); /* Blank extent marking error */

    if (legal_label_macro_name(*token, &fp_error_handler) == ERROR) /* Check if the label name is valid */
        return ERROR;
    
    if (get_comma_param_cnt(paramsPtr) != ERROR){ /* Check if parameters exist after the token and process them */
        while (*token != NULL) {
            add_extent_label(head, token, commandCode); /* Add extent label to the linked list */
            *token = strtok(NULL, PARAM_DELIMITERS); /* Move to the next token */
        }
    }

    return TRUE; /* Extent processing successful */
}

int datastring_handler(char*** dcImage, char** token, char* originalLine, int* dc, int commandCode) {
    char* paramsPtr;
    int* params;
    int paramCnt;

    /* Find the pointer to the parameters after the token */
    paramsPtr = get_param_pointer(originalLine, LAST_CHARACTER(*token));
    /* Analyze the datastring parameters and get the count and values */
    paramCnt = call_datastring_analyzer(token, &params, paramsPtr, commandCode);

    /* Check if parameters were successfully analyzed and process them */
    if (paramCnt != -1)
        add_data_word(dcImage, params, dc, paramCnt); /* Add data word to the dcImage */

    free(params); /* Free the memory allocated for parameters */
    return TRUE; /* Datastring processing successful */
}

int operation_handler(char*** icImage, label** labelTable, char** token, char* originalLine, int* ic, int* labelCount, int commandCode, char sourceSequenceArray[16][4], char destSequenceArray[16][4]) {
    int paramCnt;
    char* paramPtr;
    operation op;
    operand operand1, operand2;

    /* Find the pointer to the parameters after the token */
    paramPtr = get_param_pointer(originalLine, LAST_CHARACTER(*token));
    /* Get the count of parameters separated by commas */
    paramCnt = get_comma_param_cnt(paramPtr);

    /* Check if the number of parameters is valid */
    if (paramCnt >= 3)
        return fp_error_handler(Too_Many_Operands); /* Too many operands error */
    else if (paramCnt != ERROR) {
        op.opcode = commandCode;

        /* Get values of operand1 and operand2 and set them in the operation */
        if (get_operand_value(&operand1, *token) != ERROR && get_operand_value(&operand2, *token) != ERROR) {
            set_operands(&op, &operand1, &operand2); /* Set operands in the operation structure */
            /* Check parameter sequences and add operation words */
            if (check_param_sequence(sourceSequenceArray, op.sourceOperand, op.opcode, _SOURCE) == TRUE && check_param_sequence(destSequenceArray, op.destOperand, op.opcode, _DEST) == TRUE) {
                add_first_op_word(icImage, &op, ic); /* Add the first word of the operation to icImage */
                add_operand_words(icImage, labelTable, &op, ic, *labelCount); /* Add operand words to icImage */
            }
        }
    }    

    return TRUE; /* Operation processing successful */
}

int fp_warning_handler(int warningCode){
    switch (warningCode){
    case Label_Points_At_ExternEntry:
        printf("Warning: Label points at .extern/.entry. Assembler will ignore the label. Line: %d\n", fp_lineNum);
        break;
    case Extent_Label_Already_Defined_Similarly:
        printf("Warning: Extent label already defined with same type. Line: %d\n", fp_lineNum);
        break;
    }

    return TRUE;
}

int fp_error_handler(int errorCode){
    switch (errorCode){  
    case Illegal_Name_First_Char:
        printf("Error: Illegal label name. First character must be a letter. Line: %d\n", fp_lineNum);
        break;
    case Illegal_Name_Illegal_Chars:
        printf("Error: Illegal label name. Label name can only contain letters and numbers. Line: %d\n", fp_lineNum);
        break;
    case Illegal_Name_Saved_Word:
        printf("Error: Illegal label name. Label cant be named a saved word Line: %d\n", fp_lineNum);
        break;  
    case Unknown_Command:
        printf("Error: Unknown command in line %d\n", fp_lineNum);
        break;
    case Extra_Comma:
        printf("Error: Extra comma at the end/start in line %d\n", fp_lineNum);
        break;
    case Double_Comma:
        printf("Error: Double comma in line %d\n", fp_lineNum);
        break;
    case Missing_Comma:
        printf("Error: Missing comma. Line: %d\n", fp_lineNum);
        break;
    case Parameter_Not_Whole_Number:
        printf("Error: Non whole numerical value found. Line: %d\n", fp_lineNum);
        break;
    case Parameter_Out_Of_Bounds:
        printf("Error: Numerical parameter entered is out of bounds. Line: %d\n", fp_lineNum);
        break;
    case Invalid_Source_Sequence:
        printf("Error: Invalid source operand. Line: %d\n", fp_lineNum);
        break;
    case Invalid_Dest_Sequence:
        printf("Error: Invalid destination operand. Line: %d\n", fp_lineNum);
        break;
    case Missing_Dest_Operand:
        printf("Error: Missing destination operand. Line: %d\n", fp_lineNum);
        break;
    case Missing_Source_Operand:
        printf("Error: Missing source operand. Line: %d\n", fp_lineNum);
        break;
    case Too_Many_Operands:
        printf("Error: Too many operands passed to operation. Line: %d\n", fp_lineNum);
        break;
    case Illegal_String_Declaration:
        printf("Error: Illegal string declaration (are you missing \"\"?). Line: %d\n", fp_lineNum);
        break;
    case Label_Already_Defined:
        printf("Error: Label already defined beforehand. Line: %d\n", fp_lineNum);
        break;
    case Extent_Label_Already_Defined_Differently:
        printf("Error: Extent label already defined a different type. Line: %d\n", fp_lineNum);
        break;
    case Name_Too_Long:
        printf("Error: Label name exceeds max length. Line: %d\n", fp_lineNum);
        break;
    case Undefined_Register:
        printf("Error: Undefined register. Line: %d\n", fp_lineNum);
        break;
    case Blank_Label_Declaration:
        printf("Error: No operation/instruction found after label declaration. Line: %d\n", fp_lineNum);
        break;
    case Blank_Extent_Marking:
        printf("Error: No labels found after '.extern'/'.entry' marking. Line: %d\n", fp_lineNum);
        break;
    case Illegal_String_Char:
    	printf("Error: Illegal character entered in string. Line: %d\n", fp_lineNum);
    	break;
    case Blank_DataString_Instruction:
        printf("Error: No parameters/string found after '.data'/'string' instruction. Line: %d\n", fp_lineNum);
        break;
    }

    fp_status = ERROR;
    return ERROR;
}