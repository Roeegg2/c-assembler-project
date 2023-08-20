#include "assembler.h"

int PRINTWORDS(char **counterArray, int counter, int subCounter){
    int i;
    for (i = subCounter; i < counter + subCounter; i++){
        printf("address %d:", i + 100);
        printf(" %s\n", counterArray[i - subCounter]);
    }
    return 0;
}

int PRINTEXTENT(extentlabel *head){
    while (head != NULL){
        printf("label: %s, type: %d\n", head->labelName, head->type);
        head = head->next;
    }
    return 0;
}

int PRINTLABEL(label *labelTable, int labelCount){
    int i;
    for (i = 0; i < labelCount; i++){
        printf("label: %s, address: %d\n", labelTable[i].labelName, labelTable[i].address);
    }
    return 0;
}

int main(int argc, char** argv){
    label *labelTable;
    extentlabel *head;
    maplabel* mapHead;
    int dc, ic, labelCount, status, i, fpf;
    char **dcImage;
    char **icImage;
    char sourceSequenceArray[16][4]; 
    char destSequenceArray[16][4];
    fpf = 0;

    set_sequence_array_source(sourceSequenceArray);
    set_sequence_array_dest(destSequenceArray);

    for (i = 1; i < argc; i++){
        dcImage = (char** )malloc(sizeof(char*));
        icImage = (char** )malloc(sizeof(char*));
        labelTable = (label* )malloc(sizeof(label));
        head = NULL;
        mapHead = NULL;

        dc = ic = 0;

        status = invoke_preassembler(argv[i]);
        if (status == TRUE)
            status = invoke_firstpass(&dcImage, &icImage, &labelTable, &head, &mapHead, argv[i], &dc, &ic, &labelCount, sourceSequenceArray, destSequenceArray);
        if (status == TRUE)
            invoke_secondpass(&dcImage, &icImage, labelTable, head, mapHead, argv[i], labelCount, dc, ic, &fpf);  
        
/*         printf("-------- ext/ent list: --------\n");
        PRINTEXTENT(head);
        printf("-------- instructions words: --------\n");
        PRINTWORDS(icImage, ic, 0);
        printf("-------- data words: --------\n");
        PRINTWORDS(dcImage, dc, ic);
        printf("-------- label table: --------\n");
        PRINTLABEL(labelTable, labelCount);   */

        free(labelTable);
        free_counter_array(&dcImage, dc);
        free_counter_array(&icImage, ic);
        free_extent(head);
        free_maplabel(mapHead);
    }

    printf("\nProgram finished.\nFully processed %d/%d files.\n", fpf, i-1);
    return 0;
}

int free_extent(extentlabel* head){
    extentlabel* foo;

    while (head != NULL){
        free(head->address.addr); /*Free the addr field*/
        foo = head; /*Saving head in temp to be able to free head node*/
        head = head->next; 
        free(foo);
    }

    return TRUE;
}

int free_maplabel(maplabel* mapHead){
    maplabel* foo;

    while (mapHead != NULL){
        foo = mapHead;
        mapHead = mapHead->next;
        free(foo);
    }

    return TRUE;
}

int free_counter_array(char*** counterImage, int counter){
    int i;

    for (i = 0; i < counter; i++)
        if ((*counterImage)[i] != NULL) /*Freeing each char in word allocated*/
            free((*counterImage)[i]);
    
    free((*counterImage)); /*freeing the main pointer (not pointer to pointer)*/
    
    return TRUE;
}