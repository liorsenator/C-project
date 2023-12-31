#include "tables.h"
#include "globals.h"
#include "ast.h"

/* This function processes a line in the second pass of the assembler
 * @param lt The ast tree of the line (Abstract Syntax Tree)
 * @param table The symbol table
 * @param line The processed line information
 * @return TRUE if the line was processed successfully, FALSE otherwise*/
bool spass_line_process (struct ast lt, s_table *table, line_info line);

/*This function encodes the label into the code image
 * @param symbol_table The symbol table
 * @param code_image The code image
 * @param ic The instruction counter*/
void spass_label_encoding (s_table *symbol_table, machine_word *code_image, long ic);