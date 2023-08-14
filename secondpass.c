#include "secondpass.h"
#include "shared.h"
#include "utils.h"

/**NOTES TO SELF:
 * 1. if command is wrong but has a lable pointing at it, should i add the label or not?
*/
/* secondpass_invoker(&dcImage, &icImage, labelTable, &head, filename, dc, ic, labelCount); */
char base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\0";

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel** head, char* filename, int labelCount, int dc, int ic){
    extentlabel* temp;
    int errorFound;
    
    errorFound = FALSE;
    temp = *head;

    complete_extent_data(icImage, labelTable, head, &errorFound, ic, labelCount);
    map_labels(icImage, labelTable, &errorFound, ic, labelCount);

    if (errorFound == FALSE){
        write_extent_file(temp, filename);
        write_ob_file(*icImage, *dcImage, filename, ic, dc);
    }
    
    return TRUE;
}

  /* swap each label with its address, plus error reporting  */
int map_labels(char*** icImage, label* labelTable, int* errorFound, int ic, int labelCount){
      /* go over the icImage and if there is a label (aka an element in the array which its first char isnt 0 or 1) */
    label* foo;
    int i, boost;

    for (i = 0; i < ic; i++){
        if (*(*icImage)[i] != '0' && *(*icImage)[i] != '1'){
            foo = find_label(labelTable, (*icImage)[i], labelCount);

            if (foo != NULL){
                (*icImage)[i] = (char*)realloc((*icImage)[i], sizeof(char) * 12);

                if (foo->isData == TRUE)
                    boost = ic;
                else
                    boost = 0;
                    
                convert_to_binary((*icImage)[i], foo->address + boost, 10);
                strcat((*icImage)[i], "10");   /* adding ARE */
            }
            else{
                *errorFound = TRUE;   /* move this to error_handler() as well */
                return FALSE;   /* return error_handler() - label declared but not found */
            }
        }
    }

    return TRUE;
}

int write_ob_file(char** icArray, char** dcArray, char* filename, int ic, int dc){
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ob", "w");

    fprintf(obFile, "%d\t%d\n", ic, dc);
    for (i = 0; i < ic; i++){ /* add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(icArray[i], 0));
        fprintf(obFile, "%c\n", binary_to_base64(icArray[i], 6));
    }
    
    for (i = 0; i < dc; i++){ /* add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(dcArray[i], 0));
        fprintf(obFile, "%c\n", binary_to_base64(dcArray[i], 6));
    }
    
    fclose(obFile);

    return TRUE;
}

int write_extent_file(extentlabel* head, char* filename){
    FILE* extFile; 
    FILE* entFile;

    extFile = open_file(filename, ".ext", "w");
    entFile = open_file(filename, ".ent", "w");

    while (head != NULL){
        if (head->type == _EXTERN)
            print_extern(*head, extFile);
        else
            fprintf(entFile, "%s\t%d\n", head->labelName, head->address.addr[0]);

        head = head->next;
    }

    return TRUE;
}

int print_extern(extentlabel head, FILE* extFile){
    int i;

    for (i = 0; i < head.address.count; i++)
        fprintf(extFile, "%s\t%d\n", head.labelName, head.address.addr[i]);

    return 0;
}

int complete_extent_data(char*** icImage, label* labelTable, extentlabel** head, int* errorFound, int ic, int labelCount){
    while ((*head) != NULL){
        if ((*head)->type == _ENTRY)
            get_data_entry(labelTable, head, errorFound, labelCount, ic);
        else
            get_data_extern(icImage, head, errorFound, ic);

        (*head) = (*head)->next;
    }   

    return TRUE;
}

int get_data_entry(label* labelTable, extentlabel** head, int* errorFound, int labelCount, int ic){
    int i, boost;

    boost = 0;
    for (i = 0; i < labelCount; i++){
        if (strcmp(labelTable[i].labelName, (*head)->labelName) == 0){
            (*head)->address.addr = (int*)malloc(sizeof(int));
            (*head)->address.addr[0] = labelTable[i].address + boost;
            return TRUE;
        }
    }

    *errorFound = TRUE;   /* move this to error_handler() as well */
    return FALSE;   /* return error_handler() - no such label found  */
}

int get_data_extern(char*** icImage, extentlabel** head, int* errorFound, int ic){
    int i;
    
    for (i = 0; i < ic; i++){
        printf("%s\n", (*icImage)[i]);
        if (strcmp((*head)->labelName, (*icImage)[i]) == 0){
            (*head)->address.count++;
            (*head)->address.addr = (int*)realloc((*head)->address.addr, sizeof(int) * ((*head)->address.count));
            (*head)->address.addr[(*head)->address.count-1] = i+100;  /* not sure this is the correct address i should be assigning */ 
            strcpy((*icImage)[i], "000000000001");
        }
    }

      /* *errorFound = TRUE;   move this to error_handler() as well */
    return FALSE;   /* return error_handler() - return warning - label declared as extern but is not used in code  */
}


char binary_to_base64(char* binary, int start){
    int decimalValue, i;

    decimalValue = 0;
    for (i = start; i < 6+start; i++)
        decimalValue = (decimalValue << 1) + (binary[i] - '0');

    return base64Table[decimalValue];
}