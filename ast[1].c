#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "ast.h"

#define MAX_INT_VALUE 2047
#define  MIN_INT_VALUE -2048

/* This function gets a line and stores the directive in the ast tree.
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the directive in */
static void get_directive(const char *line, int *index, struct ast *lt);

/*
    This function gets a line and stores the string in the ast tree.
    @param line The line to parse
    @param index The index of the line to start parsing from
    @param lt The ast tree to store the string in
*/
static void get_string(const char *line, int *index, struct ast *lt);

/*This function gets a line and stores the data in the ast tree.
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the data in*/
static void get_data (const char *line, int *index, struct ast *lt);

/* This function gets a line and stores the entry/extern label in the ast tree
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the entry/extern label in */
static void get_ent_extern_label (const char *line, int *index, struct ast *lt);

/* This function gets a line and stores the instruction in the ast tree.
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the instruction in */
static void get_code_instruction (const char *line, int *index, struct ast *lt);

/* This function gets a line and stores the operands of a set A instruction in the ast tree.
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the operands in */
static void get_set_a_operands (const char *line, int *index, struct ast *lt);

/* This function gets a line and stores the operands of a set B instruction in the ast tree.
 * @param line The line to parse
 * @param index The index of the line to start parsing from
 * @param lt The ast tree to store the operands in */
static void get_set_b_operands (const char *line, int *index, struct ast *lt);

/*This function gets an operand and stores its type in the ast tree, and returns its addressing type
 * @param operand The operand to analyze
 * @param lt The ast tree to store the operand type in
 * @return The addressing type of the operand*/
static enum operand_addr_type analyze_operand (char *operand, struct ast *lt);

struct ast ast_get_ast_from_line(const char *line) {
    struct ast lt = {0}; /* The ast tree that will return*/
    char *ptr;
    int i = 0, index = 0;
    char *token = NULL;
    skip_white(line, &i);
    ptr = (char *)(line + i);
    /* Checks if the line is empty*/
    if (line[i] == '\0' || line[i] == '\n' || line[i] == EOF) {
        lt.ast_union_option = ast_union_asm_empty_line;
        return lt;
    }
    /* Checks if the line is comment*/
    else if (line[i] == ';') {
        lt.ast_union_option = ast_union_asm_comment;
        return lt;
    }
    /* Checks label definition*/
    skip_white(line, &i);
    if ((ptr = strchr(line, ':')) != NULL) {
        index = ptr - (line + i);
        token = malloc (sizeof(char) * index + 1);
        strncpy(token, line + i, index);
        token[index] = '\0';
        if (is_valid_label(token)) {
            strcpy(lt.label, token);
            free(token);
            index += (i + 1); /*Skips the ':' */
        }
        else {
            lt.ast_union_option = ast_union_ast_error;
            strcpy(lt.ast_error, "Invalid label");
            free(token);
            return lt;
        }
    }
    else {
        lt.label[0] = '\0'; /* No label*/
    }
    /*Checks if the line is a directive (.string, .data, .entry or .extern)*/
    skip_white(line, &index);
    if (line[index] == '.') {
        get_directive(line, &index, &lt);
        skip_white(line, &index);
        if (lt.ast_union_option == ast_union_ast_error) {
            return lt;
        }
        else if (lt.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_string) {
            get_string(line, &index, &lt);
            return lt;
        }
        else if (lt.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_data) {
            get_data(line, &index, &lt);
        }
        else if (lt.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_entry ||
                lt.asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option == ast_union_asm_dir_extern) {
            get_ent_extern_label(line, &index, &lt);
            return lt;
        }
    }
    else {
        get_code_instruction(line, &index, &lt);
        if (lt.ast_union_option == ast_union_ast_error) {
            return lt;
        }
            skip_white(line, &index);
        /* First set commands (2 Parameter) - mov, cmp, add, sub, lea */
        if (lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_mov ||
            lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_cmp ||
            lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_add ||
            lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_sub ||
            lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_lea) {
            get_set_a_operands(line, &index, &lt);
            return lt;
        }
        /* Second set commands - not, clr, inc, dec, jmp, bne, red, prn, jsr*/
        if (lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_not ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_clr ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_inc||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_dec ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_jmp ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_bne ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_red ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_prn ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_jsr) {
            get_set_b_operands(line, &index, &lt);
            return lt;
        }
        /* Third set commands (No parameters) - stop, rts */
        if (lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_stop ||
                lt.asm_inst_asm_dir.asm_inst.inst_name == ast_inst_rts) {
            skip_white(line, &index);
            if (line[index] != '\0' && line[index] != '\n' && line[index] != EOF) {
                lt.ast_union_option = ast_union_ast_error;
                strcpy(lt.ast_error, "Detected trailing characters, Group C instructions do not have operands");
                return lt;
            }
           return lt;
        }
    }
    return lt;
}

static void get_directive (const char *line, int *index, struct ast *lt) {
    (*index)++; /* SKips the '.' */
    if (strncmp(line + (*index), "string", 6) == 0) {
        lt->ast_union_option = ast_union_asm_dir;
        lt->asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option = ast_union_asm_dir_string;
        (*index) +=6; /* Skips the directive*/
    }
    else if (strncmp (line + (*index), "data", 4) == 0) {
        lt->ast_union_option = ast_union_asm_dir;
        lt->asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option = ast_union_asm_dir_data;
        (*index) +=4; /* Skips the directive*/
    }
    else if (strncmp(line + (*index), "entry", 5) == 0){
        lt->ast_union_option = ast_union_asm_dir;
        lt->asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option = ast_union_asm_dir_entry;
        (*index) += 5;
    }
    else if (strncmp(line+ (*index), "extern", 6) == 0) {
        lt->ast_union_option = ast_union_asm_dir;
        lt->asm_inst_asm_dir.asm_dir.ast_union_asm_dir_option = ast_union_asm_dir_extern;
        (*index) +=6; /* Skips the directive*/
    }
    else {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Invalid instruction.");
        return;
    }
}

static void get_string(const char *line, int *index, struct ast *lt) {
    char *end_quote;
    skip_white(line, &(*index));
    if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, ".string instruction must have a string.");
        return;
    }
    if (line[*index] != '"') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "String must start with quotes(\").");
        return;
    }
    (*index)++; /* Skips the '"' */
    /*Check for closing quote */
    end_quote = strchr(line + (*index), '"');
    if (end_quote == NULL) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "String must end with quotes(\").");
        return;
    }
    else {
        int len = end_quote - (line + (*index));
        strncpy(lt->asm_inst_asm_dir.asm_dir.dir.string, line + (*index), len);
        lt->asm_inst_asm_dir.asm_dir.dir.string[len] = '\0';
        (*index) += len + 1; /*Skips the '"'*/
    }
    skip_white(line, &(*index));
    if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Detected trailing characters after the end of the string.");
        return;
    }
}

static void get_data (const char *line, int *index, struct ast *lt) {
    char *endPtr;
    int value;
    int num_of_ints = 0;
    do {
        skip_white(line, &(*index));
        if (line[*index] == ',') {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Leading comma detected, must start with an integer.");
            return;
        } else if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == EOF) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, ".data instruction must have at least one integer.");
            return;
        }
        value = strtol(line + (*index), &endPtr, 10);
        if (endPtr == line + (*index)) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Invalid number.");
            return;
        }
        if (value > MAX_INT_VALUE || value < MIN_INT_VALUE) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Number out of range, will set data to 0.");
            return;
        }
        lt->asm_inst_asm_dir.asm_dir.dir.num_array.num_array[num_of_ints] = value;
        lt->asm_inst_asm_dir.asm_dir.dir.num_array.num_count++;
        num_of_ints++;
        (*index) += endPtr - (line + (*index));
        skip_white(line, &(*index));
        if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == EOF) {
            return;
        } else {
            if (line[*index] != ',') {
                lt->ast_union_option = ast_union_ast_error;
                strcpy(lt->ast_error, "Invalid character, expected ','.");
                return;
            }
            (*index)++; /* Skips the ',' */
            skip_white(line, &(*index));
            if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == EOF) {
                lt->ast_union_option = ast_union_ast_error;
                strcpy(lt->ast_error, "Trailing comma detected.");
                return;
            } else if (line[*index] == ',') {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Consecutive commas detected.");
            return;
            }
        }
    }
    while (*endPtr != '\n' || *endPtr != '\0' || *endPtr != EOF);
}

static void get_ent_extern_label (const char *line, int *index, struct ast *lt) {
    char *token = NULL;
    int len;
    skip_white(line, &(*index));
    if (lt->label[0] != '\0') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Entry/Extern instruction can't be in label definition.");
        return;
    }
    if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Entry/Extern must be followed by a label.");
        return;
    }
    for (len=0; line[*index + len] != '\0' && line[*index + len] != '\n' && line[*index + len] != EOF &&
        !(isspace(line[*index + len])); len++);
    token = (char *)malloc (sizeof(char) * (len + 1));
    strncpy(token, line + (*index), len);
    token[len] = '\0';
    (*index) += len;
    if (!is_valid_label(token)) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Invalid label.");
        free(token);
        return;
    }
    strcpy(lt->asm_inst_asm_dir.asm_dir.dir.label, token);
    free(token);
    skip_white(line, &(*index));
    if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Detected trailing characters after the end of the label.");
        return;
    }
}

static void get_code_instruction (const char *line, int *index, struct ast *lt) {
    int len;
    char *potential_command = NULL;
    struct command{char *name; enum inst_name inst;};
    struct command commands[] = {
            {"mov", ast_inst_mov},
            {"cmp", ast_inst_cmp},
            {"add", ast_inst_add},
            {"sub", ast_inst_sub},
            {"not", ast_inst_not},
            {"clr", ast_inst_clr},
            {"lea", ast_inst_lea},
            {"inc", ast_inst_inc},
            {"dec", ast_inst_dec},
            {"jmp", ast_inst_jmp},
            {"bne", ast_inst_bne},
            {"red", ast_inst_red},
            {"prn", ast_inst_prn},
            {"jsr", ast_inst_jsr},
            {"rts", ast_inst_rts},
            {"stop", ast_inst_stop}
    };
    for (len=0; line[*index + len] != '\0' && line[*index + len] != '\n' &&
        line[*index + len] != EOF && !(isspace(line[*index + len])); len++);
    potential_command = malloc(sizeof(char) * len + 1);
    strncpy(potential_command, line + *index, len);
    potential_command[len] = '\0';
    (*index) += len;
    lt->ast_union_option = ast_union_asm_inst;
    {
        struct command *ptr = NULL;
        int i;
        for (ptr = commands, i=0; i<16; i++, ptr++) {
            if(strcmp(potential_command, ptr->name) == 0) {
                lt->asm_inst_asm_dir.asm_inst.inst_name = ptr->inst;
                return;
            }
        }
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Command does not exist");
    }
}

static void get_set_a_operands (const char *line, int *index, struct ast *lt) {
    char *comma;
    int len;
    char *operand;
    if ((comma = strchr(line + *index, ',')) == NULL) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Set A command must have two operands, seperated by a comma.");
        return;
    }
    skip_white(line, &(*index));
    if (line[*index] == ',') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Leading comma detected, must start with an operand");
        return;
    }
    /* Check the presence of '@' before register*/
    if (line[*index] == 'r' && isdigit(line[*index + 1])) {
        if (*index == 0 || line[*index -1] != '@') {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "First register should be preceded by '@'");
            return;
        }
    }
    for (len = 0; line[*index + len] != '\0' && line[*index + len] != '\n' && line[*index + len] != EOF &&
            !(isspace(line[*index + len])) && line[*index + len] != ','; len++);
    /* Store the first operand*/
    operand = malloc(sizeof(char) * len + 1);
    strncpy(operand, line + *index, len);
    operand[len] = '\0';
    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[0] = analyze_operand(operand, lt);
    /*0 indicates first operand.*/
    /* Check that the first operand is valid */
    if (lt->ast_union_option == ast_union_ast_error) {
        strcpy(lt->ast_error, "First operand is illegal.");
        free(operand);
        return;
    }
    else {
        if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[0] == ast_op_addr_type_immed) {
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[0].immed = atoi(operand);
        } else if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[0] == ast_op_addr_type_label) {
            strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[0].label, operand);
        } else if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[0] == ast_op_addr_type_register) {
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[0].reg = operand[1];
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[0].reg_num = operand[2];
        }
    }
    free(operand);
    (*index) += len;
    skip_white(line, &(*index));
    (*index)++; /* Skips the comma*/
    skip_white(line, &(*index));
    /*Check the presence of '@' before the second register*/
    if (line[*index] == 'r' && isdigit(line[*index + 1])) {
        if (*index == 0 || line[*index - 1] != '@') {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Second register should be preceded by '@'");
            return;
        }
    }
    if (line[*index] == ',') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Consecutive commas detected.");
        return;
    }
    for(len=0; line[*index + len] != '\0' && line[*index + len] != '\n' && line[*index + len] != EOF &&
        !(isspace(line[*index + len])) && line[*index + len] != ','; len++);
    /* Store the second operand*/
    operand = malloc(sizeof(char) * len + 1);
    strncpy(operand, line + *index, len);
    operand[len] = '\0';
    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[1] = analyze_operand(operand, lt); /*1 indicates first operand.*/
    /* Check that the second operand is valid*/
    if (lt->ast_union_option == ast_union_ast_error) {
        strcpy(lt->ast_error, "Second operand is illegal.");
        free(operand);
        return;
    }
    else {
        if(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[1] == ast_op_addr_type_immed){
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[1].immed = atoi(operand);
        }
        else if(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[1] == ast_op_addr_type_label){
            strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[1].label, operand);
        }
        else if(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oat[1] == ast_op_addr_type_register) {
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[1].reg = operand[1];
            lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setA.oatu[1].reg_num = operand[2];
        }
    }
    free(operand);
    (*index) += len;
    /* Try to detect trailing character*/
    skip_white(line, &(*index));
    if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Detected trailing characters after the second operand.");
        return;
    }
}

static void get_set_b_operands (const char *line, int *index, struct ast *lt) {
    int len;
    char *operand;
    skip_white(line, &(*index));
    if (line[*index] == ',') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Leading comma detected, must start with an operand.");
        return;
    }
    for (len = 0; line[*index + len] != '\0' && line[*index + len] != '\n' && line[*index + len] != EOF &&
                  !(isspace(line[*index + len])); len++);
    /* Store the operand*/
    operand = malloc(sizeof(char) * len + 1);
    strncpy(operand, line + *index, len);
    operand[len] = '\0';
    (*index) += len;
    skip_white(line, &(*index));
    if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != EOF) {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Detected trailing characters after the operand. \nNote that spaces are not"
                              "allowed in group B operands, and that only one operand is allowed.");
        free(operand);
        return;
    }
    if (strchr(operand, '(') == NULL) {
        lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.oat = analyze_operand(operand, lt);
        if (lt->ast_union_option == ast_union_ast_error) {
            free(operand);
            return;
        } else {
            if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.oat == ast_op_addr_type_immed) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.oatu.immed = atoi((operand));
            } else if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.oat == ast_op_addr_type_label) {
                strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.oatu.label, operand);
            } else if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.oat == ast_op_addr_type_register) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.oatu.reg = operand[1];
            }
            free(operand);
            return;
        }
    } else { /* Label with parameters*/
        char *delim = strchr(operand, '(');
        char *token;
        int token_len = 0, i = 0;
        lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.oat = ast_op_addr_type_label;
        /* Check for closing parenthesis*/
        if (strchr(operand, ')') == NULL) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Missing closing parenthesis.");
            free(operand);
            return;
        } else {
            if (operand[strlen(operand) - 1] != ')') {
                lt->ast_union_option = ast_union_ast_error;
                strcpy(lt->ast_error, "Operand must end with closing parenthesis.");
                free(operand);
                return;
            }
        }
        /* Getting the label*/
        token_len = delim - operand;
        token = malloc((sizeof(char)) * token_len + 1);
        strncpy(token, operand, token_len);
        if (token_len == 0) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Missing label name.");
            free(operand);
            free(token);
            return;
        }
        token[token_len] = '\0';
        if (!is_valid_label(token)) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Invalid label name.");
            free(operand);
            free(token);
            return;
        } else {
            strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.label,
                   token);
            free(token);
        }
        i += token_len + 1; /* Skip the label and parenthesis*/
        /* Getting the first parameter */
        delim = strchr(operand + i, ',');
        if (delim == NULL) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Missing parameter.");
            free(operand);
            return;
        }
        token_len = delim - (operand + i);
        token = malloc((sizeof(char)) * token_len + 1);
        strncpy(token, operand + i, token_len);
        if (token_len == 0) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Missing first parameter.");
            free(operand);
            free(token);
            return;
        }
        token[token_len] = '\0';
        lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[0] =
                analyze_operand(token, lt);
        if (lt->ast_union_option == ast_union_ast_error) {
            free(operand);
            free(token);
            return;
        } else {
            if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[0] ==
                ast_op_addr_type_immed) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[0].immed = atoi(
                        token);
            } else if (
                    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[0] ==
                    ast_op_addr_type_label) {
                strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[0].label,
                       token);
            } else if (
                    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[0] ==
                    ast_op_addr_type_register) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[0].reg = token[1];
            }
            free(operand);
        }
        /* Getting the second parameter*/
        i += token_len + 1; /* Skip the first parameter and comma*/
        delim = strchr(operand + i, ')');
        token_len = delim - (operand + i);
        token = malloc((sizeof(char)) * token_len + 1);
        strncpy(token, operand + i, token_len);
        if (token_len == 0) {
            lt->ast_union_option = ast_union_ast_error;
            strcpy(lt->ast_error, "Missing second parameter.");
            free(operand);
            free(token);
            return;
        }
        token[token_len] = '\0';
        lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[1]
                = analyze_operand(token, lt);
        if (lt->ast_union_option == ast_union_ast_error) {
            free(operand);
            free(token);
            return;
        } else {
            if (lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[1] ==
                ast_op_addr_type_immed) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[1].immed = atoi(
                        token);
            } else if (
                    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[1] ==
                    ast_op_addr_type_label) {
                strcpy(lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[1].label,
                       token);
            } else if (
                    lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oat[1] ==
                    ast_op_addr_type_register) {
                lt->asm_inst_asm_dir.asm_inst.asm_inst_sets.asm_inst_setB.setB_operands.setB_param_label.oatu[1].reg = token[1];
            }
            free(token);
        }
    }
    if (operand != NULL) {
        free(operand);
    }
}
static enum operand_addr_type analyze_operand(char *operand, struct ast *lt) {
    char *endptr;
    long value;
    int reg_num;
    if (strlen(operand) == 0) {
        lt->ast_union_option = ast_union_ast_error; /* Error: operand is NULL*/
        strcpy(lt->ast_error, "Missing operand.");
        return ast_op_addr_type_error;
    }
    if (operand[0] != '@' && !isalnum(operand[0]) && operand[0] != '-') {
        lt->ast_union_option = ast_union_ast_error;
        strcpy(lt->ast_error, "Operand must start with alphanumeric character or @");
        return ast_op_addr_type_error;
    } else if (strlen(operand) == 3 && operand[0] == '@') {
        if (operand[1] != 'r') {
            lt->ast_union_option = ast_union_ast_error; /*Illegal operand*/
            strcpy(lt->ast_error, "Illegal operand - must be register after '@'.");
            return ast_op_addr_type_error;
        } else {
            reg_num = operand[2] - '0';
            if (reg_num >= 0 && reg_num <= 7) {
                return ast_op_addr_type_register;
            } else {
                lt->ast_union_option = ast_union_ast_error;
                strcpy(lt->ast_error, "Invalid register number.");
                return ast_op_addr_type_error;
            }
        }
    } else if (isdigit(operand[0]) || operand[0] == '-') {
        value = strtol(operand, &endptr, 10);
        if (endptr != operand && *endptr == '\0') {
            if (value >= -(1 << 11) && value < (1 << 11)) {
                return ast_op_addr_type_immed;
            } else {
                lt->ast_union_option = ast_union_ast_error;
                strcpy(lt->ast_error, "Out of range integer.");
                return ast_op_addr_type_error;
            }
        }
    } else if (is_valid_label(operand)) {
        return ast_op_addr_type_label;
    }
    lt->ast_union_option = ast_union_ast_error;
    strcpy(lt->ast_error, "Unrecognized operand.");
    return ast_op_addr_type_error;
}