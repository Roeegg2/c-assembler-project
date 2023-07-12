/**
 * Created by Roee Toledano on 7/6/2023.
 * This is the source code for the preassembler.
 * NOTES:
 * 1. make it so the user inputs the file name
 * 2. make code tidier and nicer
 * 3. take care of error when theres blank lines in the macro definition
*/ 

#include "preassembler.h"

/** 
 * A helper function.
 *  ADD DESCRIPTION HERE
 * */

/*/ make this piece of code nicer*/ 
int macro_code(char* line, FILE* amFile, macro* macros, int macroSize, int macroIndex){
    macros[macroIndex].code = (char*)realloc(macros[macroIndex].code, sizeof(char)*(macroSize+1));
    strcat(macros[macroIndex].code, line);
    return 0;
}

/**
 * A helper function.
 * its purpose is to check whether the piece of code is a macro, or an ordinary line of code, and write to the output file accodingly.
 * */

int write_to_file(char* token, char line[], int macroIndex, macro* macros, char* word, FILE* amFile){
    int i;
    for (i = 0; i < macroIndex+1; i++){
        if (strcmp(word, macros[i].name) == 0){
            fprintf(amFile, "%s", macros[i].code);
            return 1;
        }
    }

    fprintf(amFile, "%s", line);
    return 1;
}

/**
 * The core function of the preassembler.
 * This function loops over the source file, and using the helper functions, analyzes each line and writes to the output file accordingly.
*/
int main_pass(){
    FILE* asFile;
    FILE* amFile;

    char line[82], orgLine[82]; /*/ need to check if i can assume a max line length */
    char* token;
    macro* macros;
    int macroIndex, macroSize;

    macroIndex = -1;
    macros = NULL;

    asFile = fopen("source.as", "r"); /*/ need to change them so the user inputs the file names */
    amFile = fopen("result.am", "w");

    while (fgets(line, 30, asFile) != NULL){
        strcpy(orgLine, line);
        token = strtok(line, DELIMITERS);

        if (token != NULL && *token != ';'){
            if (strcmp(token, "mcro") == 0){ /*if the following is a macro definition*/
                macroSize = 0;
                macroIndex++;

                macros = (macro*)realloc(macros, sizeof(macro)*(macroIndex+1));
                macros[macroIndex].name = (char*)malloc(sizeof(char)*strlen(token));
                macros[macroIndex].code = (char*)calloc(strlen(token), sizeof(char)*macroSize);
                /*/ check for failed allocation/reallocation*/

                fgets(line, 30, asFile); 
                strcpy(orgLine, line); 
                token = strtok(line, DELIMITERS); 
                while (strcmp(token, "endmcro") != 0){ 
                    macroSize += strlen(orgLine); 
                    macro_code(orgLine, amFile, macros, macroSize, macroIndex); 
                    fgets(line, 30, asFile);    
                    strcpy(orgLine, line); 
                    token = strtok(line, DELIMITERS); 
                }

            }
            else
                write_to_file(token, orgLine, macroIndex, macros, token, amFile);
        }    
    }
}

int main(){
    main_pass();
    return 0;
}



