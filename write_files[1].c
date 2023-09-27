#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "tables.h"
#include "utils.h"

#define IC_INIT_VALUE 100
const char base64_table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void write_obj_file (char *filename, machine_word code_image[], long ic, data_word data_image[], long dc) {
    FILE *file_dest;
    int i, j;
    unsigned int value;
    char *obj_filename = str_allocate_cat(filename, ".ob");
    /* Try to create a new .ob file*/
    file_dest = fopen (obj_filename, "w");
    if (file_dest == NULL) {
    printf("Couldn't open the file %s.\n", obj_filename);
    free(obj_filename);
    return;
    }
    printf("Writing .ob file.\n");
    fprintf(file_dest, "\t \t%ld %ld\n", ic, dc); /*Print the header*/
    /* Print the code image */
    for (i=0; i<ic; i++) {
        if (code_image[i].word.f_word != NULL) {
            value = *(unsigned int*) code_image[i].word.f_word;
        }
        fprintf(file_dest, "0%d\t\t", i + IC_INIT_VALUE);
        for (j=0; j<2; j++) {
            unsigned char six_bits = (value >> (6 * (1 - j))) & 0x3F;
            fprintf(file_dest, "%c", base64_table[six_bits]);
        }
        fprintf(file_dest, "\n");
    }
    /* Print the data image */
    for (i=0; i<dc; i++) {
        fprintf(file_dest, "0%ld\t\t", i + ic + IC_INIT_VALUE);
        for (j = 0; j < 2; j++) {
            unsigned char six_bits = (data_image[i].data >> (6 * (1 - j))) & 0x3F;
            fprintf(file_dest, "%c", base64_table[six_bits]);
        }
        fprintf(file_dest, "\n");
    }
    /* Free the allocated memory*/
    free(obj_filename);
    fclose(file_dest);
}

void write_entry_file (char *filename, s_table *symbol_table) {
    int i;
    FILE *file_dest;
    char *entry_filename = str_allocate_cat(filename, ".ent");
    /* Try to create a .ent file*/
    file_dest = fopen (entry_filename, "w");
    if (file_dest == NULL) {
        printf("Could not open the file %s.\n", entry_filename);
        free(entry_filename);
        return;
    }
    printf("Writing .ent file.\n");
    /* Print the entry symbols*/
    for (i=0; i<TABLE_SIZE; ++i) {
        s_table_entry *entry = symbol_table->entries[i];
        if (entry == NULL) {
            continue;
        }
        for(;;) {
            if (entry->symbol_type == ENTRY_SYMBOL) {
                fprintf(file_dest, "%s\t\t%d\n", entry->symbol, (entry->symbol_addr) + 100);
            }
            if (entry->next == NULL) {
                break;
            }
            entry = entry->next;
        }
    }
    /* Free the allocated memory*/
    free(entry_filename);
    fclose(file_dest);
}

void write_extern_file (char *filename, s_table *symbol_table, machine_word code_image[], long ic) {
    int num = 0; /* Counts the number of times used an .extern instruction*/
    int i;
    FILE *file_dest;
    char *extern_filename = str_allocate_cat(filename, ".ext");
    /* Check if there was an .extern instruction in the file*/
    for (i=0; i<ic; i++) {
        if (code_image[i].label != NULL) {
            num++;
        }
    }
    if (num == 0) {
        free(extern_filename);
        return;
    }
    /* Try to create .ext file */
    file_dest = fopen (extern_filename, "w");
    if (file_dest == NULL) {
        printf("Could not open the file %s.\n", extern_filename);
        free(extern_filename);
        return;
    }
    printf("Writing .ext file.\n");
    /* Print the extern symbols*/
    for (i=0; i<ic; i++) {
        if (code_image[i].label != NULL) {
            s_table_entry *entry = get_from_s_table(symbol_table, code_image[i].label);
            if (entry->symbol_type == EXTERN_SYMBOL) {
                fprintf(file_dest, "%s\t%d\n", entry->symbol, i + IC_INIT_VALUE);
            }
        }
    }
    /* Free the allocated memory*/
    free(extern_filename);
    fclose (file_dest);
}
