#include "secondpass.h"
#include "shared.h"
#include "utils.h"

int sp_status;

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, char* filename, int labelCount, int dc, int ic, int* fpf){
    sp_status = TRUE;

    update_datalabels_addr(labelTable, labelCount, ic); /*Update data labels addresses (explained in function)*/
    complete_extent_data(icImage, labelTable, head, ic, labelCount); /*Completing the missing data*/
    map_entry_labels(icImage, labelTable, head, ic, labelCount); /*Mapping each label used as parameter to its extent/labelTable counterpart*/

    if (sp_status == TRUE){  /*If no error was found, we can write to the file!*/
        write_extent_file(labelTable, head, filename, labelCount);
        write_ob_file(*icImage, *dcImage, filename, ic, dc);
        (*fpf)++; /*Incrementing by one the number of fully processed files*/
    }
    
    return TRUE;
}

int map_entry_labels(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount){
    /* Go over the icImage and if there is a label (an element in the array which its first char isnt 0 or 1) get its encoding and swap it*/
    label* foo;
    int i;

    for (i = 0; i < ic; i++){ /*Going over the icImage*/
        if (*(*icImage)[i] != '0' && *(*icImage)[i] != '1'){ /*If word is not encoded yet*/
            foo = find_label(labelTable, (*icImage)[i], labelCount);

            if (foo != NULL){ /*If the word is indeed a label*/
                (*icImage)[i] = (char*)realloc((*icImage)[i], sizeof(char) * 13);
                CHECK_ALLOCATION_ERROR((*icImage)[i]);

                convert_to_binary((*icImage)[i], foo->address, 10); /*Get its encoding and write it*/
                /*Also switch the encoding */
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

int write_ob_file(char** icArray, char** dcArray, char* filename, int ic, int dc){
    char base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\0";
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ob", "w");
    CHECK_FILE_STATUS(obFile)

    fprintf(obFile, "%d\t%d\n", ic, dc); /*Printing ic and dc values on top*/
    
    for (i = 0; i < ic; i++){ /* MAYBE add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(base64Table, icArray[i], 0)); /*Writing 1st letter bits (0-6)*/
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, icArray[i], 6)); /*Writing 2nd letter (6-12)*/
    }
    
    for (i = 0; i < dc; i++){ /* MAYBE add error detection if there is a label unconvered */
        fprintf(obFile, "%c", binary_to_base64(base64Table, dcArray[i], 0)); /*Writing 1st letter bits (0-6)*/
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, dcArray[i], 6)); /*Writing 2nd letter bits (6-12)*/
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

    return TRUE;
}

int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount){
    FILE* extFile; 
    FILE* entFile;
    int extFlag, entFlag;

    extFlag = entFlag = FALSE;
    /* is opening and closing like this okay*/
    while (head != NULL){
        if (head->type == _EXTERN && head->address.count > 0){ /*If head is marked extern, and is mentioned as operand in program*/
            funcc(&extFile, filename, ".ext", &extFlag);
            print_extern(*head, extFile);
        }
        else if (head->type == _ENTRY){ /*If head is marked entry*/
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
        if (head->type == _ENTRY)
            get_data_entry(labelTable, head, labelCount); /*Complete entry data*/
            /*NOTE: Since I was looping over labelTable, I moved the error checking there to reduce code duplication */
        
        else{
            get_data_extern(icImage, head, ic); /*Complete extern data*/
            if (find_label(labelTable, head->labelName, labelCount) != NULL) 
                sp_error_handler(Extern_Declared, head->line); /*If marked as extern but also declared in file*/
        }

        head = head->next;
    }   

    return TRUE;
}

int get_data_entry(label* labelTable, extentlabel* head, int labelCount){
    label* foo;

    foo = find_label(labelTable, head->labelName, labelCount);
    if (foo != NULL){ /*If labejl is declared*/
        head->address.addr = (int*)malloc(sizeof(int)); /*Allocate one slot in addr - the declaration address*/
        CHECK_ALLOCATION_ERROR((head->address.addr ))

        head->address.addr[0] = foo->address;
        return TRUE;
    }

    return sp_error_handler(Entry_Not_Declared, head->line); /*If label is not declared*/
}

int get_data_extern(char*** icImage, extentlabel* head, int ic){
    int i;
    
    for (i = 0; i < ic; i++){ /*Looping over ic array (.extern can be mentioned legally only in operations)*/
        if (strcmp(head->labelName, (*icImage)[i]) == 0){
            head->address.count++;
            head->address.addr = (int*)realloc(head->address.addr, sizeof(int) * (head->address.count));
            CHECK_ALLOCATION_ERROR(head->address.addr);

            head->address.addr[head->address.count-1] = i+100;  /* Assinging i + 100 - the start memoery address of the program */ 
            strcpy((*icImage)[i], "000000000001");
        }
    }

    return TRUE;
}


char binary_to_base64(char* base64Table, char* binary, int start){
    int decimalValue, i;

    decimalValue = 0;
    for (i = start; i < 6+start; i++)
        decimalValue = (decimalValue << 1) + (binary[i] - '0');

    return base64Table[decimalValue];
}

int update_datalabels_addr(label* labelTable, int labelCount, int ic){
    int i;

    for (i = 0; i < labelCount; i++)
        if (labelTable[i].isInstruction == TRUE)
            labelTable[i].address += ic; /*If the function is a data label, adding the final value of ic 
            to get the final address of each label in memory*/

    return TRUE;
}

int sp_error_handler(int errorCode, int lineNum){
    switch (errorCode){
        case Extern_Declared:
            printf("Error: label marked 'extern' but is also declared in code. Line: %d\n", lineNum);
            break;
        case Entry_Not_Declared:
            printf("Error: label marked 'entry' but is not declared in code. Line: %d\n", lineNum);
            break;
        case Unknown_Label:
            printf("Error: unknown label is referenced in code. Line: %d\n", lineNum);
            break;
    }

    sp_status = ERROR;
    return sp_status;
}
