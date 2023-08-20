#include "secondpass.h"
#include "passes_utils.h"

int sp_status;

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, maplabel* mapHead, char* filename, int labelCount, int dc, int ic, int* fpf){
    sp_status = TRUE;

    update_datalabels_addr(labelTable, labelCount, ic); /*Update data labels addresses (explained in function)*/
    complete_extent_data(icImage, labelTable, head, ic, labelCount); /*Completing the missing data*/
    map_entry_labels(icImage, labelTable, head, mapHead, ic, labelCount); /*Mapping each label used as parameter to its extent/labelTable counterpart*/

    if (sp_status == TRUE){  /*If no error was found, we can write to the file!*/
        write_extent_file(labelTable, head, filename, labelCount);
        write_ob_file(*icImage, *dcImage, filename, ic, dc);
        (*fpf)++; /*Incrementing by one the number of fully processed files*/
    }
    
    return TRUE;
}

int map_entry_labels(char*** icImage, label* labelTable, extentlabel* head, maplabel* mapHead, int ic, int labelCount){
    /* Go over the icImage and if there is a label (an element in the array which its first char isnt 0 or 1) get its encoding and swap it*/
    label* foo;
    maplabel* temp;
    int i;

    for (i = 0; i < ic; i++){ /*Going over the icImage*/
        if (*(*icImage)[i] != '0' && *(*icImage)[i] != '1'){ /*If word is not encoded yet*/
            foo = find_label(labelTable, (*icImage)[i], labelCount);
            temp = find_maplabel(mapHead, i);

            if (foo != NULL){ /*If the word is indeed a label*/
                (*icImage)[i] = (char*)realloc((*icImage)[i], sizeof(char) * SIZE_OF_WORD);
                CHECK_ALLOCATION_ERROR((*icImage)[i]);

                memset((*icImage)[i], '\0', SIZE_OF_WORD);
                convert_to_binary((*icImage)[i], foo->address, 10); /*Get its encoding and write it*/
                strcat((*icImage)[i], RELOCATABLE); /*Adding ARE bits*/
            }
            else 
                sp_error_handler(Unknown_Label, temp->lineNum);
        }
    }
    return TRUE;
}

maplabel* find_maplabel(maplabel* head, int icAddress){
    while (head != NULL){
        if (head->icAddress == icAddress) /*If the address provided matches a labels address*/
            return head;
        head = head->next;
    }

    return NULL;
}

int write_ob_file(char** icArray, char** dcArray, char* filename, int ic, int dc){
    char base64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    FILE* obFile;
    int i;

    obFile = open_file(filename, ".ob", "w");
    CHECK_OPENING_FALIURE(obFile)

    fprintf(obFile, "%d\t%d\n", ic, dc); /*Printing ic and dc values on top*/
    
    for (i = 0; i < ic; i++){
        fprintf(obFile, "%c", binary_to_base64(base64Table, icArray[i], 0)); /*Writing 1st letter bits (0-6)*/
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, icArray[i], 6)); /*Writing 2nd letter (6-12)*/
    }
    
    for (i = 0; i < dc; i++){
        fprintf(obFile, "%c", binary_to_base64(base64Table, dcArray[i], 0)); /*Writing 1st letter bits (0-6)*/
        fprintf(obFile, "%c\n", binary_to_base64(base64Table, dcArray[i], 6)); /*Writing 2nd letter bits (6-12)*/
    }
    
    fclose(obFile);

    return TRUE;
}

int get_file_and_flag(FILE** filePtr, char* filename, char* extention, int* flag){
    if (*flag == FALSE){
        *filePtr = open_file(filename, extention, "w");
        CHECK_OPENING_FALIURE(filePtr)
        *flag = TRUE;
    }

    return TRUE;
}

int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount){
    FILE* extFile; 
    FILE* entFile;
    int extFlag, entFlag;

    extFlag = entFlag = FALSE;

    while (head != NULL){
        if (head->type == _EXTERN && head->address.count > 0){ /*If head is marked extern, and is mentioned as operand in program*/
            get_file_and_flag(&extFile, filename, ".ext", &extFlag);
            print_extern(*head, extFile);
        }
        else if (head->type == _ENTRY){ /*If head is marked entry*/
            get_file_and_flag(&entFile, filename, ".ent" , &entFlag);
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
            /*NOTE: Since I was looping over labelTable on get_data_entry anyway, I moved the error checking there to make it cleaner*/
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
        decimalValue = (decimalValue << 1) + (CHAR_TO_INT(binary[i]));

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
