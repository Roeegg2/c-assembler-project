
#include "secondpass.h"

/*-------------------------------------------- NON SECONDPASS FUNCTIONS FOR TESTING --------------------------------------*/
FILE* open_file(char* filename, char* ending, char* mode){
    char foo[MAX_FILENAME_LENGTH]; // add max for file extension as well

    strcpy(foo, filename);
    strcat(foo, ending);
    FILE* file = fopen(foo, mode); // change that "a" to mode when done testing

    return file;
}

/*------------------------------------------------------------------------------------------------------------------------*/
// note: maybe add a function like add_to_array() but instead of adding its switch_element_in_array()?

int secondpass_invoke(char*** icImage, char*** dcImage, label** labelTable, extentlabel* head, char* filename, int labelCount, int ic, int dc){
    FILE* extFile;
    FILE* entFile;
    FILE* obFile;
    int errorFound;

    errorFound = FALSE;

    complete_extent_data(icImage, *labelTable, head, &errorFound, ic, labelCount);

    
}

int map_labels(char*** icImage, label* labelTable, int ic, int labelCount){
    // go over the icImage and if there is a label (aka an element in the array which its first char isnt 0 or 1)
    int address, i;

    for (i = 0; i <= ic; i++){
        if (**(icImage[i]) != '0' && **(icImage[i]) != '1'){
            address = find_label();

            if (address != FALSE){
                *(icImage[i]) = (char*)realloc(*(icImage[i]), sizeof(char) * 12);
                convert_to_binary(address, *(icImage[i]));
            }
            else
                return FALSE; // return error_handler() - label declared but not found
        }
    }

    return TRUE;
}

int write_obj_file(char*** counterArray, label** labelTable, char* filename, int counter){
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ent", "w");

    for (i = 0; i < counter; i++) // add error detection if there is a label unconvered
        fprintf(obFile, "%s\n", (*counterArray)[i]);

    fclose(obFile);
}

int write_extent_file(extentlabel* head, char* filename){
    FILE* extFile; 
    FILE* entFile;

    extFile = open_file(filename, ".ext", "w");
    entFile = open_file(filename, ".ent", "w");

    while (head != NULL){
        if (head->type == _ENTRY)
            fprintf(entFile, "%s\t%s\n", head->labelName, head->address.declareAddr);
        else
            fprintf(extFile, "%s\t%s\n", head->labelName, head->address.referenceAddr);

        head = head->next;
    }
}

int complete_extent_data(char*** icImage, label* labelTable, extentlabel* head, int* errorFound, int ic, int labelCount){
    while (head != NULL){
        if (head->type == _ENTRY)
            get_data_entry(labelTable, head, errorFound, labelCount);
        else
            get_data_extern(icImage, head, errorFound, ic);

        head = head->next;
    }   
}

int get_data_extern(char*** icImage, extentlabel* head, int* errorFound, int ic){
    int i;

    for (i = 0; i <= ic; i++){
        if (strcmp(head->labelName, icImage[i]) == 0){
            head->address.referenceAddr = (char*)realloc(head->address.referenceAddr, sizeof(char) * strlen(icImage[i]));
            strcpy(head->address.referenceAddr, icImage[i]);
            strcpy(icImage[i], "000000000001");
            return TRUE;
        }
    }

    *errorFound = TRUE; // move this to error_handler() as well
    return FALSE; // return error_handler() - return warning - label declared as extern but is not used in code 
}


int get_data_entry(label* labelTable, extentlabel* head, int* errorFound, int labelCount){
    int i;

    for (i = 0; i < labelCount; i++){
        if (strcmp(labelTable[i].name, head->labelName) == 0){
            strcpy(head->address.declareAddr, labelTable[i].address);
            return TRUE;
        }
    }

    *errorFound = TRUE; // move this to error_handler() as well
    return FALSE; // return error_handler() - no such label found 
}