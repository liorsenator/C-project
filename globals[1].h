#ifndef _GLOBALS_H
#define _GLOBALS_H

#define MAX_FILE_NAME 80

/*Max size of a line in the program (.as file) */
#define MAX_LINE_LENGTH 80

/*The initial value that we can access to the memory*/
#define IC_INIT_VALUE 100

/*The size of the memory of our computer*/
#define MEMORY_SIZE 1024

/*The maximum size of a label*/
#define MAX_LABEL_SIZE 30

/*Defines a boolean type(TRUE/FALSE) */
typedef enum booleans {
    FALSE = 0, TRUE = 1
}bool;

/*Defines a macroister type from r0 to r7 and if not a register*/
typedef enum registers {
    r0 = 0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    NOT_REG = -1
}reg;

/*Defines the codes for the supported operations*/
typedef enum op_code{
    mov_op = 0,
    cmp_op,
    add_op,
    sub_op,
    not_op,
    clr_op,
    lea_op,
    inc_op,
    dec_op,
    jmp_op,
    bne_op,
    red_op,
    prn_op,
    jsr_op,
    rts_op,
    stop_op,
    error_op
} op_code;

/*Defines the addressing methods for the operands */
typedef enum symbol_t{
    ENTRY_SYMBOL = 0,
    DATA_SYMBOL,
    EXTERN_SYMBOL,
    CODE_SYMBOL
} symbol_t;

/*Defines a first word in an instruction*/
typedef struct first_w {
    unsigned int ARE: 2;
    unsigned int dest_op_addr: 3;
    unsigned int op_code: 4;
    unsigned int src_op_addr: 3;
} first_w;

/*Defines a machine word of operand with direct or immediate addressing*/
typedef struct imdt_drct_w {
    unsigned int ARE: 2;
    unsigned int operand: 10;
} imdt_drct_w;

/*Defines a machine word of registers operands - can contain destination register & source register*/
typedef struct register_w {
    unsigned int ARE: 2;
    unsigned int dest_register: 5;
    unsigned int src_register: 5;
} register_w;

/*Defines a general machine word - first word, immediate or direct addressing word, register word or data word*/
typedef struct machine_word {
    char *label; /*We will put here label names in the first pass*/
    union word {
        first_w *f_word;
        imdt_drct_w *id_word;
        register_w *r_word;
    } word;
} machine_word;

/*Defines a machine word containing data from .string or .data instruction*/
typedef struct data_word {
    unsigned int data: 12;
} data_word;

/*Defines a line a file and contains its info - file name, the number of the line and the content of the line */
typedef struct line_info{
    char *filename;
    int line_number;
    char *content;
} line_info;

#endif