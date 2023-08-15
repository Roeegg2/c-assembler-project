#include "preassembler.h"
#include "shared.h"
#include "utils.h"

int pa_status;

int invoke_preassembler(char* filename){
    macro* head;
    FILE* asFile;
    FILE* amFile;
    char originalLine[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH], macroName[MAX_MACRO_NAME_LENGTH];
    char* token;
    int lineNum;

    head = NULL;
    lineNum = 0;
    pa_status = TRUE;

    amFile = open_file(filename, ".am", "w");
    CHECK_FILE_STATUS(amFile);
    
    while (read_input_file(&asFile, filename, ".as", originalLine, &lineNum) == TRUE){
/*         check_line_too_long(&asFile, &status, lineNum); */
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);

        if (originalLine[0] != ';' && token != NULL){
            if (strcmp(token, "mcro") == 0){
                token = strtok(NULL, DELIMITERS);
                strcpy(macroName, token);

                check_for_macro_erros(&token, head, lineNum);
                head = add_macro_node(macroName, head);
                get_macro_code(&asFile, head, originalLine, line, &lineNum);
            }
            else if (pa_status != ERROR) /*may need to add to a temp array instead of directly writing, since errors may rise up also later on in the code*/
                write_line(&amFile, head, token, originalLine);
        }
    }

    free_macros(head);
    fclose(amFile);
    
    if (asFile != NULL)
        fclose(asFile); 

    return pa_status;
}

int write_line(FILE** amFile, macro* head, char* token, char* writeLine){
    macro* temp;

    if ((temp = find_macro(head, token)) != NULL){
        writeLine += strlen(token);
        fprintf(*amFile, "%s%s", temp->code, writeLine);
    }
    else
        fprintf(*amFile, "%s", writeLine);
    
    return TRUE;
}

  /* need to name this function */
int func(FILE** asFile, char* originalLine, char* line, char** token, int* lineNum){
    do {
        fgets(originalLine, MAX_LINE_LENGTH, *asFile);
        strcpy(line, originalLine);
        *token = strtok(line, DELIMITERS);
        (*lineNum)++;
    } while (originalLine[0] == ';' || *token == NULL);

    return TRUE;
}

int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line, int* lineNum){
    char* token;

      /* thinking of making this and a couple of other lines a function to reduce code duplication */
    func(asFile, originalLine, line, &token, lineNum);
    while (strcmp(token, "endmcro") != 0){
        head->code = (char*)realloc(head->code, (strlen(head->code) + strlen(originalLine) + 1) * sizeof(char));
        CHECK_ALLOCATION_ERROR(head->code);
        strcat(head->code, originalLine);

        func(asFile, originalLine, line, &token, lineNum);
    }
    
    token = strtok(NULL, DELIMITERS);
    if (token != NULL)
        pa_error_handler(Extratanatious_Text_After_Macro_Declaration, *lineNum);

    return TRUE;
}

int free_macros(macro* head){
    macro* foo;

    while(head != NULL) {
        free(head->code);
        foo = head; 
        head = head->next;
        free(foo); 
    }

    return TRUE;
}

macro* add_macro_node(char* macroName, macro* head){
    macro* newMacro;

    newMacro = (macro*)malloc(sizeof(macro));
    CHECK_ALLOCATION_ERROR(newMacro);

    newMacro->code = (char*)calloc(1, sizeof(char));
    CHECK_ALLOCATION_ERROR(newMacro->code);

    newMacro->next = head;
    strcpy(newMacro->name, macroName);

    return newMacro;
}

macro* find_macro(macro* head, char* token){
    if (LAST_CHARACTER(token) == ':')
        LAST_CHARACTER(token) = '\0';

    while (head != NULL){
        if (strcmp(head->name, token) == 0)
            return head;
        head = head->next;
    }

    return NULL;
}

int check_for_macro_erros(char** token, macro* head, int lineNum){
    if (find_macro(head, *token) != NULL)
        pa_error_handler(Macro_Already_Exists, lineNum);

    legal_label_macro_name(*token, lineNum, &pa_error_handler);

    (*token) = strtok(NULL, DELIMITERS);
    if (*token != NULL)
        pa_error_handler(Extratanatious_Text_After_Macro_Declaration, lineNum);

    return TRUE;
}

int pa_error_handler(int errorCode, int lineNum){
    switch (errorCode){
        case Illegal_Name_First_Char:
            printf("Error: Illegal macro name. First character must be a letter. Line %d\n", lineNum);
            break;
        case Illegal_Name_Illegal_Chars:
            printf("Error: Illegal macro name. Label name can only contain letters and numbers. Line %d\n", lineNum);
            break;
        case Illegal_Name_Saved_Word:
            printf("Error: Illegal macro name. Label cant be named a saved word Line %d\n", lineNum);
            break;  
        case Macro_Already_Exists:
            printf("Error: Macro already exists. Line %d\n", lineNum);
            break;
        case Extratanatious_Text_After_Macro_Declaration:
            printf("Error: Extratanatious text after macro declaration. Line %d\n", lineNum);
            break;
        case Line_Too_Long:
            printf("Error: Line exceeds max length. Line %d\n", lineNum);
            break;
    }

    pa_status = ERROR;
    return pa_status;
}

int legal_label_macro_name(char* name, int lineNum, int(*error_handler)(int, int)){
    if (is_extent_instruction(name) != FALSE || is_operation(name) != -1 || is_datastring_instruction(name) != FALSE)
        return error_handler(Illegal_Name_Saved_Word, lineNum);
    
    if (!(65 <= *name && *name <= 90) && !(97 <= *name && *name <= 122))
        return error_handler(Illegal_Name_First_Char, lineNum);

    if (strspn(name, "012345678abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != strlen(name))
        return error_handler(Illegal_Name_Illegal_Chars, lineNum);
    
    return TRUE;
}

/* int check_line_too_long(FILE** sourceFile, int* status, int lineNum){
    char extraChar;

    extraChar = fgetc(*sourceFile);
    if (extraChar != EOF)
        *status = pa_error_handler(Line_Too_Long, lineNum);
    
    return TRUE;
} */