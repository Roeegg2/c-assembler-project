#include "shared.h"

enum SecondpassErrors {Extern_Declared = 0, Entry_Not_Declared};

int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel** head, char* filename, int labelCount, int ic, int dc);
int map_labels(char*** icImage, label* labelTable, int ic, int labelCount);
int write_ob_file(char** icArray, char** dcArray, char* filename, char* base64Table, int ic, int dc);
int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount);
int complete_extent_data(char*** icImage, label* labelTable, extentlabel** head, int ic, int labelCount);
int get_data_extern(char*** icImage, extentlabel** head, int ic);
int get_data_entry(label* labelTable, extentlabel** head, int labelCount, int ic);
int print_extern(extentlabel head, FILE* extFile);
char binary_to_base64(char* binary, char* base64Table, int start);
int sp_error_handler(int errorCode, int lineNum);

int update_datalabels_addr(label* labelTable, int labelCount, int ic);