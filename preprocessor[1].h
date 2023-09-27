#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "globals.h"
#include "tables.h"
#include "utils.h"

/*This function works as the preprocessor of the assembler.
 * It reads the file and removes all comments,empty spaces and replaces all macros with their data.
 * @parm filename The name of the file to preprocess
 * @return TRUE if the file was preprocessed successfully and FALSE if not.
 * */
bool preprocess_file(const char *filename);
