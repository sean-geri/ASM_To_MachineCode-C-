#ifndef FUNCS_H
#define FUNC_H

#define MAX_LINE_LENGTH     81 /* Max length of a line is 80 not included \n */
#define MAX_LABEL_LENGTH    31 /* Max length of a label is 31 by the docss */
#define STRING_LENGTH       255 /* There's no usage for more */
#define MAX_OPCODE_LENGTH   12
#define MAX_FILE_LENGTH     50


#define ADDRESSING_IMMEDIDATELY          0
#define ADDRESSING_DIRECTLY              1
#define ADDRESSING_JUMP_WITH_PARAMS      2
#define ADDRESSING_DIRECT_REGISTER       3

/* Adding boolean type */
typedef int bool;
#define true  1
#define false 0

/* Information about assembler (such as opcode, register etc...) */
#define OPCODE_AMOUNT      16
#define REGISTERS_AMOUNT   8
#define DIRECTIVES_AMOUNT  4

extern char* opcodeStrings[];
extern char* registersString[];
extern char* directivesStrings[];

/* declare functions */
bool processAsmFile(char* asm_file_path);
void remove_new_line(char* str);
char *strrev(char *str);
bool is_comment(char line[MAX_LINE_LENGTH]);
int get_addressing_type(char* arg);
bool is_directly_addressing(char* arg);
bool is_direct_register_addressing(char* arg);
bool is_immedidately_addressing(char* arg);
bool is_empty(char line[MAX_LINE_LENGTH]);
bool is_data_directive(char line[MAX_LINE_LENGTH]);
bool is_entry_directive(char line[MAX_LINE_LENGTH]);
bool is_extern_directive(char line[MAX_LINE_LENGTH]);
bool is_string_directive(char line[MAX_LINE_LENGTH]);
bool is_jump_addressing(char line[MAX_LINE_LENGTH]);
void remove_spaces(char* input, char* result);
char* trim(char* str);
void get_label_name(char* asm_line, char* label);
bool is_valid_label_name(char* label_name);
void error(char* err);
void get_opcode_from_command(char* command, char* opcodeDest);
void get_value_from_args(char* valDest, char* args, int arg_num);
void get_inside_brackets(char* str, char* destValue);
void get_arg_of_jump_addressing(char* jmpCommand, char* argDest, int arg_num);

#endif