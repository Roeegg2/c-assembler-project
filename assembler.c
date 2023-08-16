#include "firstpass.h"
#include "secondpass.h"
#include "preassembler.h"
#include "utils.h"

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
    int dc, ic, labelCount, status;
    char **dcImage;
    char **icImage;
    int i, fpf;
    fpf = 0;
    
    for (i = 1; i < argc; i++){
        dcImage = (char** )malloc(sizeof(char*));
        icImage = (char** )malloc(sizeof(char*));
        labelTable = (label* )malloc(sizeof(label));
        head = NULL;

        dc = ic = 0;
        status = invoke_preassembler(argv[i]);
        if (status == TRUE)
            status = invoke_firstpass(&dcImage, &icImage, &labelTable, &head, argv[i], &dc, &ic, &labelCount);
        if (status == TRUE)
            invoke_secondpass(&dcImage, &icImage, labelTable, head, argv[i], labelCount, dc, ic, &fpf);

/*         printf("-------- ext/ent list: --------\n");
        PRINTEXTENT(head);
        printf("-------- instructions words: --------\n");
        PRINTWORDS(icImage, ic, 0);
        printf("-------- data words: --------\n");
        PRINTWORDS(dcImage, dc, ic);
        printf("-------- label table: --------\n");
        PRINTLABEL(labelTable, labelCount); */

        /* close files */
        /**
         * free dcImage
         * free icImage
         * free labelTable
        */

        free(labelTable);
        free_counter_array(&dcImage, dc);
        free_counter_array(&icImage, ic);
        free_extent(head);
    }

    printf("\nProgram finished.\nFully processed %d/%d files.\n", fpf, i-1);
    return 0;
}

/** TODO:
 * 1. fix segmenation fault when there are no 2 operands in command
 * 2. error reporting when there are more than 2 operands in command
 * 3. error reporting a,b,c,
 * 4. 
*/
