#include "shared.h"

enum SecondpassErrors {Extern_Declared = 0, Entry_Not_Declared, Unknown_Label};

/**
 * Invokes the second pass of the assembler to generate output files.
 *
 * @param dcImage Pointer to the data image array.
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Array of label entries.
 * @param head Pointer to the linked list of extent labels.
 * @param filename The name of the assembly source file.
 * @param labelCount The number of labels in the labelTable.
 * @param dc The number of data lines.
 * @param ic The number of instruction lines.
 * @param fpf Pointer to the count of processed files.
 * @return Returns TRUE if the second pass is successful.
 */
int invoke_secondpass(char*** dcImage, char*** icImage, label* labelTable, extentlabel* head, char* filename, int labelCount, int dc, int ic, int* fpf);

/**
 * Maps labels to their addresses in the instruction image array.
 *
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Array of label entries.
 * @param head Pointer to the linked list of extent labels.
 * @param ic The number of instruction lines.
 * @param labelCount The number of labels in the labelTable.
 * @return Returns TRUE if label mapping is successful.
 */
int map_labels(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount);

/**
 * Writes the output binary (ob) file.
 *
 * @param icArray Pointer to the instruction image array.
 * @param dcArray Pointer to the data image array.
 * @param filename The name of the assembly source file.
 * @param base64Table The base64 encoding table.
 * @param ic The number of instruction lines.
 * @param dc The number of data lines.
 * @return Returns TRUE if writing the ob file is successful.
 */
int write_ob_file(char** icArray, char** dcArray, char* filename, char* base64Table, int ic, int dc);

/**
 * Handles opening and closing files in the second pass.
 *
 * @param filePtr Pointer to the file pointer.
 * @param filename The name of the assembly source file.
 * @param extention The file extension to append to the filename.
 * @param flag Pointer to the flag indicating if the file is already open.
 * @return Returns TRUE if the file is successfully opened or closed.
 */
int funcc(FILE** filePtr, char* filename, char* extention, int* flag);

/**
 * Writes the extent and entry files.
 *
 * @param labelTable Array of label entries.
 * @param head Pointer to the linked list of extent labels.
 * @param filename The name of the assembly source file.
 * @param labelCount The number of labels in the labelTable.
 * @return Returns TRUE if writing the extent and entry files is successful.
 */
int write_extent_file(label* labelTable, extentlabel* head, char* filename, int labelCount);

/**
 * Prints external label information to the extent (ext) file.
 *
 * @param head Pointer to the extentlabel to print.
 * @param extFile Pointer to the extent (ext) file.
 * @return Returns 0.
 */
int print_extern(extentlabel head, FILE* extFile);

/**
 * Completes extent label data by updating addresses and generating data entries.
 *
 * @param icImage Pointer to the instruction image array.
 * @param labelTable Array of label entries.
 * @param head Pointer to the linked list of extent labels.
 * @param ic The number of instruction lines.
 * @param labelCount The number of labels in the labelTable.
 * @return Returns TRUE if completing extent data is successful.
 */
int complete_extent_data(char*** icImage, label* labelTable, extentlabel* head, int ic, int labelCount);

/**
 * Generates data entries for extent labels marked as entry.
 *
 * @param labelTable Array of label entries.
 * @param head Pointer to the extentlabel to process.
 * @param labelCount The number of labels in the labelTable.
 * @param ic The number of instruction lines.
 * @return Returns TRUE if generating data entries is successful.
 */
int get_data_entry(label* labelTable, extentlabel* head, int labelCount, int ic);

/**
 * Generates data entries for extent labels marked as extern.
 *
 * @param icImage Pointer to the instruction image array.
 * @param head Pointer to the extentlabel to process.
 * @param ic The number of instruction lines.
 * @return Returns TRUE if generating extern data entries is successful.
 */
int get_data_extern(char*** icImage, extentlabel* head, int ic);

/**
 * Converts a binary string to a base64 character.
 *
 * @param base64Table The base64 encoding table.
 * @param binary The binary string to convert.
 * @param start The starting index in the binary string.
 * @return Returns the base64 character.
 */
char binary_to_base64(char* base64Table, char* binary, int start);

/**
 * Updates the addresses of data labels after the first pass.
 *
 * @param labelTable Array of label entries.
 * @param labelCount The number of labels in the labelTable.
 * @param ic The number of instruction lines.
 * @return Returns TRUE if updating data label addresses is successful.
 */
int update_datalabels_addr(label* labelTable, int labelCount, int ic);

/**
 * Handles second pass error codes and displays corresponding error messages.
 *
 * @param errorCode The error code to handle.
 * @param lineNum The line number where the error occurred.
 * @return Returns the sp_status after handling the error.
 */
int sp_error_handler(int errorCode, int lineNum);
