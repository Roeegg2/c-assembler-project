#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 100
#define MAX_LABEL_LENGTH 31

typedef struct label{
    char name[MAX_LABEL_LENGTH]; /*the name of the label, eg "MAIN"*/
    int line; /*the line the table was defined*/ 
    counter cnt; /*the type of the table relocateable, */ /*the value of the counter*/
} label;

typedef struct counter{
    // not sure i need both fields, change later on if you see you need
    int counterType;
    int address;
} counter;

FILE* open_file(char* filename, char* ending, char* mode){
    char foo[MAX_FILENAME_LENGTH]; // add max for file extension as well

    strcpy(foo, filename);
    strcat(foo, ending);
    FILE* file = fopen(foo, mode); // change that "a" to mode when done testing

    return file;
}

int print_data(char** data, int dc){
    int i;
    for (i = 0; i < dc; i++){
        printf("%s\n", data[i]);
    }
}

int print_instructions(char** array, label* labelTable, char* filename, int ic){
    FILE* obFile;
    char binary[13];
    int i, labelAddress;

    obFile = open_file(filename, ".ob", "a+");

    for (i = 0; i < ic; i++){
        if (*(array[i]) == 'l') { //NOTE FOR FIRSTPASS: add the 'l' character to each label added to the array 
            labelAddress = find_label(array[i], labelTable);

            if (labelAddress != FALSE){
                get_one_word(binary, labelAddress);
                array[i] = (char*)realloc((*array), sizeof(char) * (strlen(binary)+1));
                strcpy(array[i], binary);
            }
            else{
                printf("Error: label %s not found\n", array[i]);
                return 0;
            }
        }
        else{
            fprintf(obFile, "%s\n", array[i]);
        }
    }
    return 1;
}

int main(int argc, char** argv){
    FILE* amFile;
    label* labelTable;
    char filename[MAX_FILENAME_LENGTH];
    int dc, ic;
    char** dcArray; 
    char** icArray;

    int i;
    for (i = 1; i < argc; i++){
        strcpy(filename, argv[i]);
        
        dcArray = (char**)malloc(sizeof(char*));
        icArray = (char**)malloc(sizeof(char*));
        labelTable = (label*)malloc(sizeof(label));

        first_pass_invoker(&dcArray, &icArray, &amFile, &labelTable, filename, &dc, &ic);

        printf("data words:\n");
        PRINTWORDS(dcArray, dc);
        printf("instructions words:\n");
        PRINTWORDS(icArray, ic);
    }
    // close files
    // free memory
    
}

/**
 * go over the operationArray:
 *     if found a label:
 *          go over the label table and find the label. if label found:
 *              switch the label name and the label address
 *         else:
 *              print error
 *    else:
 *        print to file
 * 
 * go over the dataArray and write it to obj file
*/