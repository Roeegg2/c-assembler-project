#include "secondpass.h"
#include "shared.h"
#include "utils.h"

/**NOTES TO SELF:
 * 1. if command is wrong but has a lable pointing at it, should i add the label or not?
*/
/* secondpass_invoker(&dcImage, &icImage, labelTable, &head, filename, dc, ic, labelCount); */
int sp_status;

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, char* filename, int labelCount, int dc, int ic){
    char base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\0";
    int sp_status;
    
    sp_status = TRUE;

    update_datalabels_addr(labelTable, labelCount, ic);
    complete_extent_data(icImage, labelTable, head, ic, labelCount);
    map_labels(icImage, labelTable, ic, labelCount);

    if (sp_status != ERROR){
        write_extent_file(labelTable, head, filename, labelCount);
        write_ob_file(*icImage, *dcImage, filename, base64Table, ic, dc);
    }
    
    return TRUE;
}

  /* swap each label with its address, plus error reporting  */
int map_labels(char*** icImage, label* labelTable, int ic, int labelCount){
      /* go over the icImage and if there is a label (aka an element in the array which its first char isnt 0 or 1) */
    label* foo;
    int i;

    for (i = 0; i < ic; i++){
        if (*(*icImage)[i] != '0' && *(*icImage)[i] != '1'){
            foo = find_label(labelTable, (*icImage)[i], labelCount);

            if (foo != NULL){
                (*icImage)[i] = (char*)realloc((*icImage)[i], sizeof(char) * 12);
                    
                convert_to_binary((*icImage)[i], foo->address, 10);
                strcat((*icImage)[i], "10");   /* adding ARE */
            }
            else{
                sp_status = ERROR;   /* move this to error_handler() as well */
                return FALSE;   /* return error_handler() - label declared but not found */
            }
        }
    }

    return TRUE;
}

int write_ob_file(char** icArray, char** dcArray, char* filename, char* base64Table, int ic, int dc){
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ob", "w");

    fprintf(obFile, "%d\t%d\n", ic, dc);
    for (i = 0; i < ic; i++){ /* add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(base64Table, icArray[i], 0));
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, icArray[i], 6));
    }
    
    for (i = 0; i < dc; i++){ /* add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(base64Table, dcArray[i], 0));
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, dcArray[i], 6));
    }
    
    fclose(obFile);

    return TRUE;
}

int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount){
    FILE* extFile; 
    FILE* entFile;

    extFile = open_file(filename, ".ext", "w");
    entFile = open_file(filename, ".ent", "w");

    while (head != NULL){
        if (head->type == _EXTERN){
            if (find_label(labelTable, head->labelName, labelCount) == NULL)
                print_extern(*head, extFile);
            else
                sp_error_handler(Extern_Declared, head->line); /* label marked as extern but is also declared in code */
        }
        else{
            if (find_label(labelTable, head->labelName, labelCount) != NULL)
                fprintf(entFile, "%s\t%d\n", head->labelName, head->address.addr[0]);
            else
                sp_error_handler(Entry_Not_Declared, head->line); /* label marked as .entry but isnt declared in code*/
        }

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

int complete_extent_data(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount){
    while (head != NULL){
        if (head->type == _ENTRY)
            get_data_entry(labelTable, head, labelCount, ic);
        else
            get_data_extern(icImage, head, ic);

        head = head->next;
    }   

    return TRUE;
}

int get_data_entry(label* labelTable, extentlabel* head, int labelCount, int ic){
    int i;

    for (i = 0; i < labelCount; i++){
        if (strcmp(labelTable[i].labelName, head->labelName) == 0){
            head->address.addr = (int*)malloc(sizeof(int));
            head->address.addr[0] = labelTable[i].address;
            return TRUE;
        }
    }

    return TRUE;
    /* *sp_status = TRUE;   */ /* move this to error_handler() as well */
    /* return FALSE; */   /* return error_handler() - no such label found  */
}

int get_data_extern(char*** icImage, extentlabel* head, int ic){
    int i;
    
    for (i = 0; i < ic; i++){
        printf("%s\n", (*icImage)[i]);
        if (strcmp(head->labelName, (*icImage)[i]) == 0){
            head->address.count++;
            head->address.addr = (int*)realloc(head->address.addr, sizeof(int) * (head->address.count));
            head->address.addr[head->address.count-1] = i+100;  /* not sure this is the correct address i should be assigning */ 
            strcpy((*icImage)[i], "000000000001");
        }
    }

    return TRUE;
      /* *sp_status = TRUE;   move this to error_handler() as well */
      /* return error_handler() - return warning - label declared as extern but is not used in code  */
}


char binary_to_base64(char* base64Table, char* binary, int start){
    int decimalValue, i;

    decimalValue = 0;
    for (i = start; i < 6+start; i++)
        decimalValue = (decimalValue << 1) + (binary[i] - '0');

    return base64Table[decimalValue];
}

/* DONT LIKE THIS FUNCTION, CHANGE THE WHOLE MECHANISM */
int update_datalabels_addr(label* labelTable, int labelCount, int ic){
    int i;

    for (i = 0; i < labelCount; i++)
        if (labelTable[i].isData == TRUE)
            labelTable[i].address += ic;

    return TRUE;
}

int sp_error_handler(int errorCode, int lineNum){
    switch (errorCode){
        case Extern_Declared:
            printf("Error: label marked as extern but is also declared in code. Line %d\n", lineNum);
            break;
        case Entry_Not_Declared:
            printf("Error: label marked as .entry but is not declared in code. Line %d\n", lineNum);
            break;
    }

    sp_status = ERROR;
    return sp_status;
}