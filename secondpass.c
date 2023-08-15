#include "secondpass.h"
#include "shared.h"
#include "utils.h"

/**NOTES TO SELF:
 * 1. if command is wrong but has a lable pointing at it, should i add the label or not?
*/
/* secondpass_invoker(&dcImage, &icImage, labelTable, &head, filename, dc, ic, labelCount); */
int sp_status;

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, char* filename, int labelCount, int dc, int ic, int* fpf){
    char base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\0";
    
    sp_status = TRUE;

    update_datalabels_addr(labelTable, labelCount, ic);
    complete_extent_data(icImage, labelTable, head, ic, labelCount);
    map_labels(icImage, labelTable, head, ic, labelCount);

    if (sp_status == TRUE){
        write_extent_file(labelTable, head, filename, labelCount);
        write_ob_file(*icImage, *dcImage, filename, base64Table, ic, dc);
        (*fpf)++;
    }
    
    return TRUE;
}

  /* swap each label with its address, plus error reporting  */
int map_labels(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount){
      /* go over the icImage and if there is a label (aka an element in the array which its first char isnt 0 or 1) */
    label* foo;
    int i;

    for (i = 0; i < ic; i++){
        if (*(*icImage)[i] != '0' && *(*icImage)[i] != '1'){
            foo = find_label(labelTable, (*icImage)[i], labelCount);

            if (foo != NULL){
                (*icImage)[i] = (char*)realloc((*icImage)[i], sizeof(char) * 13);
                    
                convert_to_binary((*icImage)[i], foo->address, 10);
                (*icImage)[i][10] = '1';  /* adding ARE */
                (*icImage)[i][11] = '0';
                (*icImage)[i][12] = '\0'; 
            }
 /* The error we are checking: if an unknown label is used as an operand  
 Doesnt work on some cases because we only loop over icArray, not dcArray            
 else if (find_extent_label(head, (*icImage)[i]) == NULL)
                sp_error_handler(Unknown_Label, 999);  need to change lineNum here  */
        }
    }
    return TRUE;
}

int write_ob_file(char** icArray, char** dcArray, char* filename, char* base64Table, int ic, int dc){
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ob", "w");
    CHECK_FILE_STATUS(obFile)

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

int funcc(FILE** filePtr, char* filename, char* extention, int* flag){
    if (*flag == FALSE){
        *filePtr = open_file(filename, extention, "w");
        CHECK_FILE_STATUS(filePtr)
        *flag = TRUE;
    }
}

int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount){
    FILE* extFile; 
    FILE* entFile;
    int extFlag, entFlag;

    extFlag = entFlag = FALSE;
    /* is opening and closing like this okay*/
    while (head != NULL){
        if (head->type == _EXTERN && head->address.count > 0){
            funcc(&extFile, filename, ".ext", &extFlag);
            print_extern(*head, extFile);
        }
        else if (head->type == _ENTRY){
            funcc(&entFile, filename, ".ent" , &entFlag);
            fprintf(entFile, "%s\t%d\n", head->labelName, head->address.addr[0]);
        }

        head = head->next;
    }
    
    if (extFlag == TRUE)
        fclose(extFile);
    if (entFlag == TRUE)
        fclose(entFile);

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
        if (head->type == _ENTRY){
            get_data_entry(labelTable, head, labelCount, ic);
            if (find_label(labelTable, head->labelName, labelCount) == NULL)
                sp_error_handler(Entry_Not_Declared, head->line);
        }
        else{
            get_data_extern(icImage, head, ic);
            if (find_label(labelTable, head->labelName, labelCount) != NULL)
                sp_error_handler(Extern_Declared, head->line);
        }

        head = head->next;
    }   

    return TRUE;
}

int get_data_entry(label* labelTable, extentlabel* head, int labelCount, int ic){
    label* foo;

    foo = find_label(labelTable, head->labelName, labelCount);
    if (foo != NULL){
        head->address.addr = (int*)malloc(sizeof(int));
        head->address.addr[0] = foo->address;
        return TRUE;
    }

    return TRUE;
    /* *sp_status = TRUE;   */ /* move this to error_handler() as well */
    /* return FALSE; */   /* return error_handler() - no such label found  */
}

int get_data_extern(char*** icImage, extentlabel* head, int ic){
    int i;
    
    for (i = 0; i < ic; i++){
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
            printf("Error: label marked 'extern' but is also declared in code. Line %d\n", lineNum);
            break;
        case Entry_Not_Declared:
            printf("Error: label marked 'entry' but is not declared in code. Line %d\n", lineNum);
            break;
        case Unknown_Label:
            printf("Error: unknown label is referenced in code. Line %d\n", lineNum);
            break;
    }

    sp_status = ERROR;
    return sp_status;
}
