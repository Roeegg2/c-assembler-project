#include "preassembler.h"
#include "firstpass.h"
#include "secondpass.h"
#include "shared_fp_pa.h"
#include "shared_fp_sp.h"

/**
 * This function frees memory occupied by the extent label linked list.
 *
 * @param head Pointer to the head of the extent label linked list.
 * @return The success status of freeing the extent label linked list.
 */
int free_extent(extentlabel* head);

/**
 * This function frees memory occupied by the map label linked list.
 * 
 * @param mapHead Pointer to the head of the map label linked list.
 * @return The success status of freeing the map label linked list.
*/

int free_maplabel(maplabel* mapHead);

/**
 * This function frees memory occupied by the counterArray.
 *
 * @param counterImage Pointer to the counterArray.
 * @param counter Number of elements in the counterArray.
 * @return The success status of freeing the counterArray.
 */
int free_counter_array(char*** counterImage, int counter);