#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "tables.h"
#ifndef UTILS_H
#define UTILS_H

/*Skips white spaces in a string by telling the index where the white spaces end*/
void skip_white(const char *string, int *index);

/*#define SKIP_WHITE (string, index) while (isspace((string)[(index)])){(index++); } Made a change here from chatGPT !!*/
/* Concatenates two strings and allocates memory to the new string
 * @param first_str The first string to concatenate
 * @param second_str The second string to concatenate
 * @return A pointer to the concatenated string */
char *str_allocate_cat (char *first_str, char *second_str);

/*Splits a string in to words by defined seperators
 * @param string A pointer to the string to be splitted into words
 * @param seperators By which seperators to split the string
 * @param count A pointer to a variable to hold the number of worlds in the splitted string
 * @return A pointer to the first word in the splitted string */
char **split_string (char *string, char *seperators, int *count);

/* Free the memory allocated to a devided string
 * @param splitted_string The splitted string to get free
 * @param count The number of words in the string */
void free_splitted_string (char **splitted_string, int count);

/* Prints an error message to the standard output
 * @param line The line where the error occured (contains information about the file name and content of the line)
 * @param format The formatted message to print
 * @param ... The variables needed to format the message (Like in printf) Check-!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
void print_error_msg(line_info *line, const char *format, ...);

/*Prints a data word to the standard output
 * @param word The data word to print */
void print_dw(data_word word);

void print_mw(machine_word word);

/* Free the code image
 * @param code_image The code image to be free
 * @param ic The number of values in the list */
void free_code_image(machine_word code_image[], long ic);

/*Creates a new machine word
 * @return A pointer to the new machine word */
machine_word *create_machine_word();

/*Updates the data symbols address in the symbol table
 * @param table The symbol to update
 * @param ic The number of words in the code image */
void update_data_symbols_address(s_table *table, long ic);

/* Checks if a string is a valid label
 * @param word The string to check
 * @return TRUE if the string is all digits, FALSE otherwise */
bool is_all_digits(const char *operand);

/*Checks if a string is a valid label
 * @param word The string to check
 * @return TRUE if the string is a valid label, FALSE otherwise */
bool is_valid_label(char *word);

#endif /*UTILS_H*/