#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define MEMORY_SIZE                2000  /* TODO: check the memory size later */
#define DATA_DIRECTIVE_STR_LENGTH    5     /* the string .data is 5 chars */
#define STRING_DIRECTIVE_STR_LENGTH  7     /* the string .string is 7 chars */
#define ENTRY_DIRECTIVE_STR_LENGTH   6     /* the string .entry is 6 chars */
#define EXTERN_DIRECTIVE_STR_LENGTH  7     /* the string .extern is 7 chars */
#define START_OF_ADDRESS            100
#define SIZE_OF_BYTE                14
#define ASM_VALUE_OF_0              '.'
#define ASM_VALUE_OF_1              '/'

typedef union {
    int number;
    struct {
        int destAddressing;
        int sourceAddressing;
        int opcode;
        int param1;
        int param2;
        char sourceValue[MAX_LABEL_LENGTH];
        char destValue[MAX_LABEL_LENGTH];
    } command;
} MemoryWord;

void set_data_directive(char label_name[MAX_LABEL_LENGTH], char* values);
void set_string_directive(char label_name[MAX_LABEL_LENGTH], char* values);
void handle_entry(char label_name[MAX_LABEL_LENGTH], char* values);
void handle_extern(char label_name[MAX_LABEL_LENGTH], char* values);
void handle_command(char label_name[MAX_LABEL_LENGTH], char* command);
MemoryWord get_memory_of_command(int opcodeIndex, char* arg1, char* arg2);
int opCodeParamsCount(int opcodeIndex);
int get_param_value_of_address_type(int address_type);
void process_asm(char* asm_file_name);

#endif