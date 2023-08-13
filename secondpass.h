#include "shared.h"

int secondpass_invoker(char*** dcImage, char*** icImage, label* labelTable, extentlabel** head, char* filename, int labelCount, int ic, int dc);
int map_labels(char*** icImage, label* labelTable, int* errorFound, int ic, int labelCount);
int write_ob_file(char** counterArray, label* labelTable, char* filename, int counter);
int write_extent_file(extentlabel* head, char* filename);
int complete_extent_data(char*** icImage, label* labelTable, extentlabel** head, int* errorFound, int ic, int labelCount);
int get_data_extern(char*** icImage, extentlabel** head, int* errorFound, int ic);
int get_data_entry(label* labelTable, extentlabel** head, int* errorFound, int labelCount);
int print_extern(extentlabel head, FILE* extFile);