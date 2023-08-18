#include "preassembler.h"
#include "shared.h"
#include "utils.h"

int pa_status;
int pa_lineNum;

int invoke_preassembler(char* filename){
    macro* head;
    FILE* asFile;
    FILE* amFile;
    char originalLine[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH], macroName[MAX_MACRO_NAME_LENGTH];
    char* token;

    head = NULL;
    pa_lineNum = 0;
    pa_status = TRUE;

    amFile = open_file(filename, ".am", "w");
    
    while (read_input_file(&asFile, filename, ".as", originalLine, &pa_lineNum) == TRUE){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);

        if (originalLine[0] != ';' && token != NULL){ /*If the current line isn't blank and isnt a comment (a line we should write)*/
            if (strcmp(token, "mcro") == 0){ /*If its the start of a macro declaration*/
                token = strtok(NULL, DELIMITERS);
                strcpy(macroName, token); /*Get macro name*/

                check_for_macro_erros(&token, head);
                head = add_macro_node(macroName, head); /*Adding the macro node*/
                get_macro_code(&asFile, head, originalLine, line); /*Adding the macro code*/
            }
            else if (pa_status != ERROR) /*SELFNOTE: may need to add to a temp array instead of directly writing, since errors may rise up also later on in the code*/
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

    temp = find_macro(head, token); /*Check if first word in this line is a macro call*/
    if (temp != NULL){
        writeLine += strlen(token)+1; /*Getting a pointer to the code after macro declaration (if there is any)*/
        fprintf(*amFile, "%s%s", temp->code, writeLine); /*Printing both*/
    }
    else
        fprintf(*amFile, "%s", writeLine); /*If its not a macro call, just write the line as is*/
    
    return TRUE;
}

int func(FILE** asFile, char* originalLine, char* line, char** token){
    do {
        fgets(originalLine, MAX_LINE_LENGTH, *asFile);
        strcpy(line, originalLine);
        *token = strtok(line, DELIMITERS);
        pa_lineNum++;
    } while (originalLine[0] == ';' || *token == NULL);

    return TRUE;
}

int get_macro_code(FILE** asFile, macro* head, char* originalLine, char* line){
    char* token;

    func(asFile, originalLine, line, &token);
    while (strcmp(token, "endmcro") != 0){ /*If we havent reached "endmcro keyword" (meaning there is still more macro code to add)*/
        head->code = (char*)realloc(head->code, (strlen(head->code) + strlen(originalLine) + 1) * sizeof(char));
        CHECK_ALLOCATION_ERROR(head->code);
        
        strcat(head->code, originalLine); /*Concatenate the new code to the macro-code field*/

        func(asFile, originalLine, line, &token);
    }
    
    token = strtok(NULL, DELIMITERS);
    if (token != NULL) /*If there is some text after "endmcro" keyword*/
        pa_error_handler(Extratanatious_Text_After_Macro_Declaration);

    return TRUE;
}

int free_macros(macro* head){
    macro* foo;

    while(head != NULL) {
        free(head->code); /*Free the code field*/
        foo = head; /*Saving head in temp to be able to free head node*/
        head = head->next;
        free(foo); 
    }

    return TRUE;
}

macro* add_macro_node(char* macroName, macro* head){
    macro* newMacro;

    newMacro = (macro*)malloc(sizeof(macro)); /*Allocatjng memory for the new node */
    CHECK_ALLOCATION_ERROR(newMacro);
    newMacro->code = (char*)calloc(1, sizeof(char)); /*Allocating memory for macro code */
    CHECK_ALLOCATION_ERROR(newMacro->code);

    newMacro->next = head; /*Part of setting the new node as head */
    strcpy(newMacro->name, macroName); /*Copying macro name*/

    return newMacro;
}

macro* find_macro(macro* head, char* token){
    if (LAST_CHARACTER(token) == ':')
        LAST_CHARACTER(token) = '\0';

    while (head != NULL){
        if (strcmp(head->name, token) == 0) /*If we found a matching macro*/
            return head;
        head = head->next;
    }

    return NULL;
}

int check_for_macro_erros(char** token, macro* head){
    if (find_macro(head, *token) != NULL) /*If a macro with the same name has already been defined*/
        pa_error_handler(Macro_Already_Exists);

    legal_label_macro_name(*token, &pa_error_handler); /*Check for additional macro errors*/

    (*token) = strtok(NULL, DELIMITERS);
    if (*token != NULL) /*If there is code after mcro declaration */
        pa_error_handler(Extratanatious_Text_After_Macro_Declaration);

    return TRUE;
}

int pa_error_handler(int errorCode){
    switch (errorCode){
        case Illegal_Name_First_Char:
            printf("Error: Illegal macro name. First character must be a letter. Line: %d\n", pa_lineNum);
            break;
        case Illegal_Name_Illegal_Chars:
            printf("Error: Illegal macro name. Label name can only contain letters and numbers. Line: %d\n", pa_lineNum);
            break;
        case Illegal_Name_Saved_Word:
            printf("Error: Illegal macro name. Label cant be named a saved word. Line: %d\n", pa_lineNum);
            break;  
        case Macro_Already_Exists:
            printf("Error: Macro already exists. Line: %d\n", pa_lineNum);
            break;
        case Extratanatious_Text_After_Macro_Declaration:
            printf("Error: Extratanatious text after macro declaration. Line: %d\n", pa_lineNum);
            break;
        case Line_Too_Long:
            printf("Error: Line: exceeds max length. Line: %d\n", pa_lineNum);
            break;
    }

    pa_status = ERROR;
    return pa_status;
}

int legal_label_macro_name(char* name, int(*error_handler)(int)){
    /* If label/macro are a saved word*/
    if (is_extent_instruction(name) != FALSE || is_operation(name) != -1 || is_datastring_instruction(name) != FALSE)
        return error_handler(Illegal_Name_Saved_Word);
    
    /*If label/macro first letter isnt a letter*/
    if (!(65 <= *name && *name <= 90) && !(97 <= *name && *name <= 122))
        return error_handler(Illegal_Name_First_Char);

    /*If label/macro contain a character that isnt a latin letter or a number*/
    if (strspn(name, "012345678abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != strlen(name))
        return error_handler(Illegal_Name_Illegal_Chars);
    
    return TRUE;
}

int check_line_too_long(char* line){
    int i;

    /*still a problem here when in the last line of the file you dont hit enter*/
    for (i = 0; i < strlen(line)-1; i++)
        if (line[i] == '\n' || line[i] == '\0')
            return TRUE;

    if (line[i] == '\n')
        return TRUE;

    pa_error_handler(Line_Too_Long);
    return FALSE;
}
