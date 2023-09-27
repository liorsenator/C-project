/* This file creates the ast for the assembler
 * The ast is a recursive descent parser that creates an abstract syntax tree of the line. */

#ifndef assembler_ast
#define assembler_ast

#define MAX_LABEL_SIZE 30
#define MAX_ast_ERROR_BUFF_LEN 150
#define MAX_STRING_LEN 80
#define MAX_NUMBER_SIZE 80

enum inst_name {
    /* Set A commands*/
    ast_inst_mov, ast_inst_cmp, ast_inst_add,ast_inst_sub, ast_inst_lea,
    /* Set B commands */
    ast_inst_not, ast_inst_clr, ast_inst_inc, ast_inst_dec, ast_inst_jmp, ast_inst_bne, ast_inst_red, ast_inst_prn,
    ast_inst_jsr,
    /* Set C commands */
    ast_inst_rts, ast_inst_stop
};

enum operand_addr_type {
    ast_op_addr_type_error = -1,
    ast_op_addr_type_immed = 1,
    ast_op_addr_type_label=3,
    ast_op_addr_type_register=5
};

union operand_addr_type_u {
    char label[MAX_LABEL_SIZE + 1];
    char reg;
    char reg_num;
    int immed;
};

struct ast {
    char label[MAX_LABEL_SIZE + 1];
    char ast_error[MAX_ast_ERROR_BUFF_LEN + 1];
    enum {
        ast_union_ast_error = -1,
        ast_union_asm_inst,
        ast_union_asm_dir, /* .dir, .string, .extern, .entry*/
        ast_union_asm_empty_line,
        ast_union_asm_comment
    } ast_union_option;
    union {
        struct {
            enum {
                ast_union_asm_dir_string,
                ast_union_asm_dir_data,
                ast_union_asm_dir_entry,
                ast_union_asm_dir_extern
            } ast_union_asm_dir_option;
            union {
                char string[MAX_STRING_LEN + 1];
                struct {
                    int num_array[MAX_NUMBER_SIZE];
                    int num_count;
                } num_array;
                char label[MAX_LABEL_SIZE + 1];
            } dir;
        } asm_dir;

        struct {
            enum inst_name inst_name;
            union {
                struct {
                    enum operand_addr_type oat[2];
                    union operand_addr_type_u oatu[2];
                } asm_inst_setA;
                struct {
                    enum operand_addr_type oat;
                    union {
                        struct {
                            char label[MAX_LABEL_SIZE + 1];
                            enum operand_addr_type oat[2];
                            union operand_addr_type_u oatu[2];
                        } setB_param_label;
                        union operand_addr_type_u oatu;
                    } setB_operands;
                } asm_inst_setB;
            } asm_inst_sets;
        } asm_inst;
    }asm_inst_asm_dir;
};

/*This function gets a line and returns an abstract syntax tree of the line.
 * @param line The line to parse
 * @return The abstract syntax tree of the line (struct ast) */
struct ast ast_get_ast_from_line (const char *line);
#endif
