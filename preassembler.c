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
int add_macro_codelines(FILE** asFile, char* code, char line[], char originalLine[]){
    char* token;

    do {
        fgets(line, MAX_LINE_LENGTH, *asFile);
        strcpy(originalLine, line);
        token = strtok(line, DELIMITERS);
    }
    while (token == NULL);

    while (strcmp(token, "endmcro") != 0){
        if (*token != ';'){ /* can there be any comments in macro definition? */
            code = (char*)realloc(code, sizeof(char) * (strlen(code) + strlen(originalLine) + 1));
            strcat(code, originalLine);
            do {
                fgets(line, MAX_LINE_LENGTH, *asFile);
                strcpy(originalLine, line);
                token = strtok(line, DELIMITERS);
            }
            while (token == NULL);
        }
    }
    return 0;
}

macro* add_macro_node(macro* head, char* name){
    macro* newMacro;
    newMacro = (macro*)malloc(sizeof(macro));

    if (head != NULL)
        head->next = NULL;

    newMacro->next = head;
    newMacro->name = (char*)calloc(strlen(name)+1, sizeof(char));
    newMacro->code = (char*)calloc(1, sizeof(char));

    strcpy(newMacro->name, name);

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
int preassembler_invoke(FILE** asFile, FILE** amFile, char* filename){
    macro* head;
    char* token;
    char line[MAX_LINE_LENGTH], originalLine[MAX_LINE_LENGTH];
    int lineNum;

    lineNum = 0;
    head = NULL;
    
    (*amFile) = open_file(filename, ".am", "w");
    while (read_input_file(asFile, filename, ".as", originalLine, &lineNum)){
        strcpy(line, originalLine);
        token = strtok(line, DELIMITERS);
        if (token != NULL && *token != ';'){
            if (strcmp(token, "mcro") == 0){
                token = strtok(NULL, DELIMITERS);
                head = add_macro_node(head, token);
                add_macro_codelines(asFile, head->code, line, originalLine);
            }
            else
                write_line(amFile, head, token, originalLine, line);
        }
    }
    free_macros(head);

    fclose(*asFile); 
    fclose(*amFile);
}

int main(){
    FILE* asFile;
    FILE* amFile;

    preassembler_invoke(&asFile, &amFile, "test");
    return 0;
}