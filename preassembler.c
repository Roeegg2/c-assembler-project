#include "preassembler.h"

int write_line(FILE* amFile, macro* head, char originalLine[], char line[]){
    char* token;

    token = strtok(line, DELIMITERS);
    while (head != NULL){
        if (strcmp(head->name, token) == 0){
            fprintf(amFile, "%s", head->code);
            return 0;
        }
        head = head->next;
    }
    fprintf(amFile, "%s", originalLine);
    return 0;
}

/* make this piece of code nicer. */
int add_macro_codelines(FILE* asFile, char* code, char line[], char originalLine[]){
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
        }
    }
    return 0;
}

macro* add_macro_node(macro* head, char* name){
    macro* newMacro;
    newMacro = (macro*)malloc(sizeof(macro));

    newMacro->next = head;
    newMacro->name = (char*)malloc(sizeof(char) * strlen(name));
    newMacro->code = (char*)calloc(1, sizeof(char*));

    strcpy(newMacro->name, name);

    return newMacro;
}

/*handles the input and output file*/
int file_handler(){
    FILE* asFile;
    FILE* amFile;

    macro* head;

    char* token;
    char line[MAX_LINE_LENGTH],  originalLine[MAX_LINE_LENGTH];

    head = NULL;
    asFile = fopen("source.as", "r"); /*/ need to change them so the user inputs the file names */
    amFile = fopen("result.am", "w");

    while (fgets(line, 30, asFile) != NULL){
        strcpy(originalLine, line);
        token = strtok(line, DELIMITERS);

        if (token != NULL && *token != ';'){
            if (strcmp(token, "mcro") == 0){
                token = strtok(NULL, DELIMITERS);
                head = add_macro_node(head, token);
                add_macro_codelines(asFile, head->code, line, originalLine);
            }
            else
                write_line(amFile, head, originalLine, line);
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    file_handler();
    return 0;
}