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

    if ((temp = find_macro(head, token)) != NULL)
        fprintf(*amFile, "%s", temp->code);
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
        error_handler(Extratanatious_Text_After_Macro_Declaration, *lineNum);

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
    while (head != NULL){
        if (strcmp(head->name, token) == 0)
            return head;
        head = head->next;
    }

    return NULL;
}

int check_for_macro_erros(char** token, macro* head, int lineNum){
    if (find_macro(head, *token) != NULL)
        error_handler(Macro_Already_Exists, lineNum);

    (*token) = strtok(NULL, DELIMITERS);
    if (*token != NULL)
        error_handler(Extratanatious_Text_After_Macro_Declaration, lineNum);

    /* if (is_operation() != FALSE || is_guidanceshit() != FALSE || is_guidancelabelshit() != FALSE)
        *status = error_handler(Macro_Name_Is_Saved_Word, lineNum);  macro name is a saved word, which isnt right */

    return TRUE;
}

int error_handler(int errorCode, int lineNum){
    switch (errorCode){
        case Macro_Already_Exists:
            printf("Error: Macro already exists. Line %d\n", lineNum);
            break;
        case Extratanatious_Text_After_Macro_Declaration:
            printf("Error: Extratanatious text after macro declaration. Line %d\n", lineNum);
            break;
        case Macro_Name_Is_Saved_Word:
            printf("Error: Macro name is a saved word. Line %d\n", lineNum);
            break;
        case Line_Too_Long:
            printf("Error: Line exceeds max length. Line %d\n", lineNum);
            break;
        default:
            printf("Error: Unknown error. Line %d\n", lineNum);
            break;
    }

    pa_status = ERROR;
    return pa_status;
}

/* int check_line_too_long(FILE** sourceFile, int* status, int lineNum){
    char extraChar;

    extraChar = fgetc(*sourceFile);
    if (extraChar != EOF)
        *status = error_handler(Line_Too_Long, lineNum);
    
    return TRUE;
} */