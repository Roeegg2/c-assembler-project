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

int main(int argc, char **argv){
    label *labelTable;
    extentlabel *head;
    char filename[MAX_FILENAME_LENGTH];
    int dc, ic, labelCount;
    char **dcImage;
    char **icImage;

    int i;
    for (i = 1; i < argc; i++){
        strcpy(filename, argv[i]);

        dcImage = (char **)malloc(sizeof(char *));
        icImage = (char **)malloc(sizeof(char *));
        labelTable = (label *)malloc(sizeof(label));
        head = NULL;

        invoke_preassembler(filename);
        invoke_firstpass(&dcImage, &icImage, &labelTable, &head, filename, &dc, &ic, &labelCount);

        printf("ext/ent list:\n");
        PRINTEXTENT(head);
        printf("instructions words:\n");
        PRINTWORDS(icImage, ic, 0);
        printf("data words:\n");
        PRINTWORDS(dcImage, dc, ic);
        printf("label table:\n");
        PRINTLABEL(labelTable, labelCount);

        // close files
        // free memory
    }
}