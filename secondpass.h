#include "shared.h"

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel** head, char* filename, int labelCount, int ic, int dc);
int map_labels(char*** icImage, label* labelTable, int* errorFound, int ic, int labelCount);
int write_ob_file(char** icArray, char** dcArray, char* filename, int ic, int dc);
int write_extent_file(extentlabel* head, char* filename);
int complete_extent_data(char*** icImage, label* labelTable, extentlabel** head, int* errorFound, int ic, int labelCount);
int get_data_extern(char*** icImage, extentlabel** head, int* errorFound, int ic);
int get_data_entry(label* labelTable, extentlabel** head, int* errorFound, int labelCount, int ic);
int print_extern(extentlabel head, FILE* extFile);
char binary_to_base64(char* binary, int start);