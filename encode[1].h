#include <stdlib.h>
#include "ast.h"
#include "tables.h"
#include "globals.h"

/* This function encodes an instruction line the code image.
 * @param line The line to encode
 * @param lt The ast tree of the line (abstract syntax tree)
 * @param code_image The code image to encode the line into
 * @param ic The instruction counter
 * @param symbol_table The symbol table
 * @return TRUE if the line was encoded successfully, FALSE otherwise */
bool encode_instruction(line_info *line, struct ast *lt, machine_word *code_image, long *ic, s_table *symbol_table);