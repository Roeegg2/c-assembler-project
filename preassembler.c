#include "preassembler.h"

int write_line(FILE** amFile, macro* head, char* token, char originalLine[], char line[]){
    while (head != NULL){
        if (strcmp(head->name, token) == 0){
            fprintf(*amFile, "%s", head->code);
            return 0;
        }
        head = head->next;
    }
    
    fprintf(*amFile, "%s", originalLine);
    return 0;
}

/* make this piece of code nicer. */
int add_macro_codelines(FILE** asFile, char* code, char* line, char* originalLine, int* lineNum){
    char* token;

    do {
        fgets(line, MAX_LINE_LENGTH, asFile);
        strcpy(originalLine, line);
        token = strtok(line, DELIMITERS);
    }
    while (token == NULL);

    while (strcmp(token, "endmcro") != 0){
        if (*token != ';'){ /* can there be any comments in macro definition? */
            code = (char*)realloc(code, sizeof(char) * (strlen(code) + strlen(originalLine) + 1));
            strcat(code, originalLine);

            do {
                fgets(line, MAX_LINE_LENGTH, asFile);
                strcpy(originalLine, line);
                token = strtok(line, DELIMITERS);
            }
            while (token == NULL);

            (*lineNum)++;
        }
    }

    token = strtok(NULL, DELIMITERS);
    if (token != NULL)
        error_handler(Extratanatious_Text_After_Macro_Declaration, lineNum);

    return TRUE;
}

int error_handler(int errorCode, int lineNum){
    switch (errorCode){
    case Extratanatious_Text_After_Macro_Declaration:
        printf("Error: Extratanatious text after macro declaration on line %d\n", lineNum);
        break;
    case Macro_Already_Exists:
        printf("Error: Macro already exists on line %d\n", lineNum);
        break;
    }

    return 1;
}


macro* add_macro_node(macro* head, char** name, int* errorStatus, int lineNum){
    macro* newMacro;
    newMacro = (macro*)malloc(sizeof(macro));

    if (find_macro(*name) == TRUE)
        *errorStatus = error_handler(Macro_Already_Exists, lineNum); // macro already exists

    if (head != NULL)
        head->next = NULL;

    newMacro->next = head;
    newMacro->name = (char*)calloc(strlen(*name)+1, sizeof(char));
    newMacro->code = (char*)calloc(1, sizeof(char));

    *name = strtok(NULL, DELIMITERS);
    if (*name != NULL)
        *errorStatus = error_handler(Extratanatious_Text_After_Macro_Declaration, lineNum); // some extra characters after macro declaration
    
    if (is_operation() != FALSE || is_guidanceshit() != FALSE || is_guidancelabelshit() != FALSE)
        *errorStatus = error_handler(Macro_Name_Is_Saved_Word, lineNum); // macro name is a saved word, which isnt right
        
    strcpy(newMacro->name, *name);

    return newMacro;
}

FILE* open_file(char* filename, char* ending, char* mode){
    char foo[MAX_FILENAME_LENGTH]; // add max for file extension as well

    strcpy(foo, filename);
    strcat(foo, ending);
    FILE* file = fopen(foo, mode); // change that "a" to mode when done testing

    return file;
}

int read_input_file(FILE** sourceFile, char* filename, char ending[3], char line[MAX_LINE_LENGTH], int* lineNum){
    if (*lineNum == 0)    
        *sourceFile = open_file(filename, ending, "r"); 

    if (*sourceFile == NULL)
        return -1;

    if (fgets(line, MAX_LINE_LENGTH, *sourceFile) != NULL){
        (*lineNum)++;
        return 1;
    }

    return 0;
}

int free_macros(macro* head){
    macro* temp;
    while(head != NULL) {
        free(head->name); 
        free(head->code);
        temp = head; 
        head = head->next;
        free(temp); 
    }
    return 0;
}

/*handles the input and output file*/
int preassembler_invoke(char* filename){
    FILE* asFile; 
    FILE* amFile;
    macro* head;
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    int lineNum, errorFound;

    lineNum = 0;
    head = NULL;
    
    amFile = open_file(filename, ".am", "w");
    while (read_input_file(&asFile, filename, ".as", originalLine, &lineNum) == TRUE){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);

        if (token != NULL && *token != ';'){
            if (strcmp(token, "mcro") == 0){
                token = strtok(NULL, DELIMITERS);
                head = add_macro_node(head, token, &errorFound, lineNum);
                add_macro_codelines(&asFile, head->code, line, originalLine, &lineNum);
            }
            else
                write_line(&amFile, head, token, originalLine, line);
        }

    }

    free_macros(head);
    fclose(asFile); 
    fclose(amFile);
}

int main(){
    preassembler_invoke("test");
    return 0;
}