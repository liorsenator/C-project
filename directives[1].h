#include "globals.h"

/* This function receives a string and encodes it to the data image.
 * @param str The string to be encoded
 * @param data_image The data image to be encoded to
 * @param dc The data counter */
void encode_string (char str[], data_word *data_image, long *dc);

/* This function receives an array of integers and encodes it to the data image.
 * @param nu_array The array of integers to be encoded
 * @param num_count The number of integers in the array
 * @param data_image The data image to be encoded to
 * @param dc The data counter */
void encode_data (int num_array[], int num_count, data_word *data_image, long *dc);

