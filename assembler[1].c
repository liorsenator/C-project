#include <stdlib.h>
#include <stdio.h>
#include "preprocessor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "write_files.h"
#include "tables.h"
#include "globals.h"
#include "utils.h"
#include "ast.h"

/*This function receives a file name and process it to machine code
 * @param filename The file to be processed
 * @return TRUE if the file process succeeded and FALSE otherwise
 * Authors: Lior Senator and Saar Daniel
 */
bool assemble_and_generate_output (char *filename);

int main (int argc, char *argv[]) {
    int i;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 file2 file3 ...\n", argv[0]);
        exit (EXIT_FAILURE);
    }
    for (i=1; i<argc; i++) {
        if (strlen(argv[i]) > MAX_FILE_NAME) {
            fprintf(stderr, "Filename %s is too long (max %d characters)\n", argv[i], MAX_FILE_NAME);
            continue;
        }
        assemble_and_generate_output(argv[i]);
    }
    return 0;
}

bool assemble_and_generate_output (char *filename) {
    long ic = 0, dc = 0;
    data_word data_image[MEMORY_SIZE] = {0}; /* Contains the data image of the file*/
    machine_word code_image[MEMORY_SIZE] = {0}; /* Contains the code image of the file*/
    char *am_filename = NULL;
    char tmp_line[MAX_LINE_LENGTH + 2];
    line_info current_line;
    s_table *symbol_table = NULL;
    FILE *file_desc = NULL;
    bool success = TRUE, extern_sym = FALSE, entry_sym = FALSE;
    char temp_c;
    struct ast line_structure;
    /* Extract macros from the source file and incorporate them */
    if (!preprocess_file(filename)) {
        printf("Could not spread the macros correctly into %s.as\n", filename);
        return FALSE;
    }
    /* Open the preprocessed file */
    am_filename = str_allocate_cat(filename, ".am");
    file_desc = fopen(am_filename, "r");
    if (file_desc == NULL) {
        printf("Could not open the file %s.\n", am_filename);
        free(am_filename);
        return FALSE;
    }
    /* First pass: Symbol table construction and basic syntax analysis */
    symbol_table = create_s_table();
    current_line.filename = am_filename;
    current_line. content = tmp_line;
    for (current_line.line_number=1; fgets(tmp_line, MAX_LINE_LENGTH + 2, file_desc) != NULL; current_line.line_number++) {
        /* Handle line length errors */
        if (strchr(tmp_line, '\n') == NULL && !feof(file_desc)) {
            print_error_msg(&current_line, "The line is too long, maximum size of an input line is 80 characters.");
            success = FALSE;
            /* Skip excessive characters */
            do {
                temp_c = fgetc(file_desc);
            } while (temp_c != '\n' && temp_c != EOF);
        }
        else {
            line_structure = ast_get_ast_from_line(tmp_line);
            if (line_structure.ast_union_option == ast_union_ast_error) {
                print_error_msg(&current_line, line_structure.ast_error);
                success = FALSE;
            }
            else {
                if (line_structure.ast_union_option == ast_union_asm_dir) {
                    if (line_structure.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_entry)
                        entry_sym = TRUE;
                    else if (line_structure.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_extern)
                        extern_sym = TRUE;
                }
                success = first_pass_line_process (current_line, symbol_table, &ic, &dc, data_image, code_image, &line_structure);
            }
        }
    }
    update_data_symbols_address(symbol_table, ic);
    /* Second pass: Final processing and label encoding */
    rewind (file_desc);
    if (success) {
        for (current_line.line_number =1; fgets(tmp_line, MAX_LINE_LENGTH + 2, file_desc) != NULL; current_line.line_number++) {
            line_structure = ast_get_ast_from_line(tmp_line); /*Syntax error detected in the first pass*/
            success = spass_line_process(line_structure, symbol_table, current_line);
        }
        spass_label_encoding(symbol_table, code_image, ic);
    }
    /* Verify memory size limitations */
    if (ic + dc > MEMORY_SIZE - 100) { /*First 100 memory cells reserved for the system.*/
        print_error_msg(&current_line, "The memory size is too small for the file.");
        success = FALSE;
    }
    /* Generate output files if successful */
    if (success) {
        write_obj_file (filename, code_image, ic, data_image, dc);
        if (entry_sym)
            write_entry_file(filename, symbol_table);
        if (extern_sym)
            write_extern_file(filename, symbol_table, code_image, ic);
    }

    /*Free all the allocated memory*/
    free(am_filename);
    fclose(file_desc);
    free_s_table(symbol_table);
    free_code_image(code_image, ic);
    return TRUE;
}