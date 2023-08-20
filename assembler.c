#include "assembler.h"

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