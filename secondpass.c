
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

int secondpass_invoke(char*** icArray, char*** dcArray, label** labelTable, extentlabel* head, char* filename, int labelCount, int ic, int dc){
    FILE* extFile;
    FILE* entFile;
    FILE* obFile;
    int errorFound;

    errorFound = FALSE;

    unknown_labels(icArray, labelTable, &errorFound, labelCount, ic);
    get_extent_addresses(labelTable, head, &errorFound, labelCount);

    if (errorFound == FALSE){
        write_extent_file(head, extFile, entFile, filename);
        write_obj_file(icArray, labelTable, obFile, filename, ic); // ic
        write_obj_file(dcArray, labelTable, obFile, filename, dc); // dc
    }

    return TRUE;
}

int write_obj_file(char*** counterArray, label** labelTable, FILE* obFile, char* filename, int counter){
    int i;

    obFile = open_file(filename, ".ent", "w");
    for (i = 0; i < counter; i++)
        fprintf(obFile, "%s\n", (*counterArray)[i]);
}

int write_extent_file(extentlabel* head, FILE* extFile, FILE* entFile, char* filename){
    extFile = open_file(filename, ".ext", "w");
    entFile = open_file(filename, ".ent", "w");

    while (head != NULL){
        if (head->type == _ENTRY)
            fprintf(entFile, "%s %d\n", head->name, head->address);
        else
            fprintf(extFile, "%s %d\n", head->name, head->address);

        head = head->next;
    }
}


int unknown_labels(char*** icArray, label** labelTable, int* errorFound, int labelCount, int ic){
    int address, i;

    for(i = 0; i < ic; i++){
        if (*((*icArray)[i]) != '0' && *((*icArray)[i]) != '1'){
            address = find_label(labelTable, (*icArray)[i], labelCount);

            if (address == FALSE)
                error_handler(errorFound);
            else
                (*icArray)[i] = convert_to_binary(address, 13);
        }
    }

    return TRUE;
}

int get_extent_addresses(label** labelTable, extentlabel* head, int* errorFound, int labelCount){
    int address;

    while (head != NULL){
        address = find_label(labelTable, head->name, labelCount);

        if (address == FALSE)
            error_handler(errorFound);
        else
            head->address = address;

        head = head->next;
    }

    return TRUE;
}