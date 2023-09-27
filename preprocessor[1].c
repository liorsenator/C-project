#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "tables.h"
#include "utils.h"

#define MAX_MACRO_NAME_SIZE 30

bool preprocess_file(char *filename) {
    bool reading_macro = FALSE;
    int num_of_words_in_line = 0, index = 0;
    char current_line[MAX_LINE_LENGTH + 2];
    char **splitted_line;
    char *macro_name = NULL;
    char *am_filename = str_allocate_cat(filename, ".am");
    char *original_filename = str_allocate_cat(filename, ".as");
    FILE *am_file_dest;
    FILE *origin_file_dest = fopen(original_filename, "r");
    m_table *macros_table = create_m_table();

    /*Verify we were able to open the original file*/
    if (origin_file_dest == NULL) {
        printf("Could not open the file %s\n", original_filename);
        free(am_filename);
        free(original_filename);
        free_m_table(macros_table);
        return FALSE;
    }
    /*Verify we were able to create a new am file*/
    am_file_dest = fopen(am_filename, "w+");
    if (am_file_dest == NULL) {
        printf("Could not open a file named %s.am\n", am_filename);
        free(am_filename);
        free(original_filename);
        fclose(origin_file_dest);
        fclose(am_file_dest);
        free_m_table(macros_table);
        return FALSE;
    }
    while (fgets(current_line, MAX_LINE_LENGTH + 2, origin_file_dest)) {
        index = 0; /* Index to check where white space ends*/
        skip_white(current_line, &index);
        /* If the line is empty, EOF or comment then skip it */
        if (current_line[index] == '\n' || current_line[index] == EOF || current_line[index] == ';'
        || current_line[index] == '\0')
            continue;
        /*Split the line from where whitespace ends*/
        splitted_line = split_string(current_line + index, " ", &num_of_words_in_line);
        /* If defined macro, print it*/
        if (get_from_m_table(macros_table, splitted_line[0]) != NULL) {
            fputs(get_from_m_table(macros_table, splitted_line[0]), am_file_dest);
        }
        /* If there is a macro definition*/
        else if (strcmp(splitted_line[0], "mcro") == 0) {
            reading_macro = TRUE;
            macro_name = (char *)malloc(sizeof(char) * MAX_MACRO_NAME_SIZE);
            strcpy(macro_name, splitted_line[1]);
            add_m_entry(macros_table, macro_name, "");
        }
        /* If there is an end of macro definition*/
        else if (strcmp(splitted_line[0], "endmcro") == 0) {
            reading_macro = FALSE;
        }
        /* If still a macro definition*/
        else if (reading_macro == TRUE) {
            char *updated_macro = str_allocate_cat(get_from_m_table(macros_table, macro_name),
                                                   current_line);
            add_m_entry (macros_table, macro_name, updated_macro);
            free(updated_macro);
        }
        /* Regular line*/
        else {
            fputs (current_line, am_file_dest);
        }
        /* Free the memory of splitted line before iteration to the next line*/
        free_splitted_string(splitted_line, num_of_words_in_line);
    }
    /* Free all memory that we have used in the file*/
    free(am_filename);
    free(original_filename);
    fclose(origin_file_dest);
    fclose(am_file_dest);
    free_m_table(macros_table);
    if (macro_name != NULL) {
        free(macro_name);
    }
    return TRUE;
}