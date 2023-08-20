#include "shared_fp_sp.h"

enum SecondpassErrors {Extern_Declared = 0, Entry_Not_Declared, Unknown_Label};

/**
 * This function invokes the second pass of the assembly process, updating data labels' addresses,
 * completing extent data, mapping entry labels, and writing the output files if no errors are found.
 *
 * @param dcImage Pointer to the data counter image.
 * @param icImage Pointer to the instruction counter image.
 * @param labelTable Pointer to the label table.
 * @param head Pointer to the extent label linked list.
 * @param mapHead Pointer to the maplabel linked list.
 * @param filename Name of the source file.
 * @param labelCount Number of labels in the label table.
 * @param dc Data counter value.
 * @param ic Instruction counter value.
 * @param fpf Pointer to the number of fully processed files.
 * @return The success status of invoking the second pass.
 */
int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, maplabel* mapHead, char* filename, int labelCount, int dc, int ic, int* fpf);
/**
 * This function maps entry labels to their corresponding address in the instruction counter image.
 *
 * @param icImage Pointer to the instruction counter image.
 * @param labelTable Pointer to the label table.
 * @param head Pointer to the extent label linked list.
 * @param mapHead Pointer to the maplabel linked list.
 * @param ic Instruction counter value.
 * @param labelCount Number of labels in the label table.
 * @return The success status of mapping entry labels.
 */
int map_entry_labels(char*** icImage, label* labelTable, extentlabel* head, maplabel* mapHead, int ic, int labelCount);
/**
 * This function writes the output object file containing the encoded instruction and data words.
 *
 * @param icArray Pointer to the instruction counter image.
 * @param dcArray Pointer to the data counter image.
 * @param filename Name of the source file.
 * @param ic Instruction counter value.
 * @param dc Data counter value.
 * @return The success status of writing the object file.
 */
int write_ob_file(char** icArray, char** dcArray, char* filename, int ic, int dc);

/**
 * This function opens a file and assigns it to a file pointer if not already opened.
 *
 * @param filePtr Pointer to the file pointer.
 * @param filename Name of the file.
 * @param extention File extension.
 * @param flag Flag indicating whether the file is already opened.
 * @return The success status of opening the file.
 */
int get_file_and_flag(FILE** filePtr, char* filename, char* extention, int* flag);

/**
 * This function writes extent and entry data to the corresponding .ext and .ent files.
 *
 * @param labelTable Pointer to the label table.
 * @param head Pointer to the extent label linked list.
 * @param filename Name of the source file.
 * @param labelCount Number of labels in the label table.
 * @return The success status of writing the extent and entry files.
 */
int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount);

/**
 * This function prints extent label information to the external file.
 *
 * @param head Pointer to the extent label linked list.
 * @param extFile Pointer to the external file.
 * @return The success status of printing extent label information.
 */
int print_extern(extentlabel head, FILE* extFile);

/**
 * This function completes extent data by updating the addresses of extent labels and handling .entry labels.
 *
 * @param icImage Pointer to the instruction counter image.
 * @param labelTable Pointer to the label table.
 * @param head Pointer to the extent label linked list.
 * @param ic Instruction counter value.
 * @param labelCount Number of labels in the label table.
 * @return The success status of completing extent data.
 */
int complete_extent_data(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount);

/**
 * This function gets the address of an entry label from the label table and updates the extent label's address.
 *
 * @param labelTable Pointer to the label table.
 * @param head Pointer to the extent label linked list.
 * @param labelCount Number of labels in the label table.
 * @return The success status of getting entry label data.
 */
int get_data_entry(label* labelTable, extentlabel* head, int labelCount);

/**
 * This function gets the address of an extern label from the instruction counter image and updates the extent label's address.
 *
 * @param icImage Pointer to the instruction counter image.
 * @param head Pointer to the extent label linked list.
 * @param ic Instruction counter value.
 * @return The success status of getting extern label data.
 */
int get_data_extern(char*** icImage, extentlabel* head, int ic);

/**
 * This function converts a binary string to a base64 character using a base64Table.
 *
 * @param base64Table Base64 encoding table.
 * @param binary Binary string to convert.
 * @param start Start index of the binary string.
 * @return The base64 character.
 */
char binary_to_base64(char* base64Table, char* binary, int start);

/**
 * This function updates the addresses of data labels in the label table by adding the final value of ic.
 *
 * @param labelTable Pointer to the label table.
 * @param labelCount Number of labels in the label table.
 * @param ic Instruction counter value.
 * @return The success status of updating data label addresses.
 */
int update_datalabels_addr(label* labelTable, int labelCount, int ic);

/**
 * This function handles and reports errors specific to the second pass, setting the sp_status flag accordingly.
 *
 * @param errorCode Error code indicating the type of error.
 * @param lineNum Line number where the error occurred.
 * @return The status indicating whether an error occurred.
 */
int sp_error_handler(int errorCode, int lineNum);


/**
 * This function finds a label in the maplabel linked list.
 * 
 * @param head Pointer to the maplabel linked list.
 * @param icAddress Address of the label to find.
 * @return Pointer to the maplabel node containing the label.
*/
maplabel* find_maplabel(maplabel* head, int icAddress);
