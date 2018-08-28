#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "funcs.h"
#include "assembler.h"
#include "structs.h"

char* opcodeStrings[]; /* Declartion in funcs.h and define in funcs.c */

MemoryWord dataImage[MEMORY_SIZE];
int dataImagePointer = 0;

MemoryWord codeImage[MEMORY_SIZE];
int codeImagePointer = 0;

void handle_command(char label_name[MAX_LABEL_LENGTH], char* command) {
    int i, opcodeIndex = -1;
    int paramsRequired;
    char opcode[MAX_OPCODE_LENGTH];
    char arg1[MAX_LABEL_LENGTH] = "", arg2[MAX_LABEL_LENGTH] = "";
    int arg1_length, arg2_length;

    command = trim(command);
    get_opcode_from_command(command, opcode);

    for(i = 0; i < OPCODE_AMOUNT; i++) {
        if(strcmp(opcodeStrings[i], opcode) == 0) {
            opcodeIndex = i;
            break;
        }
    }

    if(opcodeIndex == -1) {
        printf("[Error] unknown command \"%s\"\r\n", opcode);
        error_mod();
        return;
    }

    paramsRequired = opCodeParamsCount(opcodeIndex); /* How many params required for the opcode */
    command = command + strlen(opcode);
    command = trim(command); /* The value of params */

    get_value_from_args(arg1, command, 1);
    get_value_from_args(arg2, command, 2);

    arg1_length = strlen(arg1);
    arg2_length = strlen(arg2);

    if(paramsRequired == 0 && (arg1_length != 0 || arg2_length != 0)) {
        printf("[Error] command '%s' not suppose to get params but got param: %s\r\n", opcode, command);
        error_mod();
        return;
    } else if(paramsRequired == 1 && (arg1_length == 0 || arg2_length > 0)) {
        printf("[Error] command '%s' supoose to get only 1 param!\r\n", opcode);
        error_mod();
        return;
    } else if(paramsRequired == 2 && (arg1_length == 0 || arg2_length == 0)) {
        printf("[Error] command '%s' supoose to get 2 params!\r\n", opcode);
        error_mod();
        return;
    }

    /* Validate that arg of jump  is only for jmp,bne,jsr command */
    if(get_addressing_type(arg1) == ADDRESSING_JUMP_WITH_PARAMS) {
        if(strcmp("jmp", opcode) != 0 && strcmp("bne", opcode) != 0 && strcmp("jsr", opcode) != 0) {
            printf("[Error] jump addressing is only for jmp, bne and jsr commands, but \"%s\" command got jump addressing!\r\n", opcode);
            error_mod();
            return;
        }
    }

    /* Saving command to code image */
    save_label(label_name, codeImagePointer, code_label);
    codeImage[codeImagePointer++] = get_memory_of_command(opcodeIndex, arg1, arg2);
}

MemoryWord get_memory_of_command(int opcodeIndex, char* arg1, char* arg2) {
    int addressing_type_arg1, addressing_type_arg2;
    MemoryWord memWord;
    int paramsRequired;

    paramsRequired = opCodeParamsCount(opcodeIndex);
    addressing_type_arg1 = get_addressing_type(arg1);
    addressing_type_arg2 = get_addressing_type(arg2);

    if(addressing_type_arg1 == -1 && strlen(arg1) != 0) {
        printf("[Error] Unknown addressing type of arg %s\r\n", arg1);
        error_mod();
        return memWord;
    }
    if(addressing_type_arg2 == -1 && strlen(arg2) != 0) {
        printf("[Error] Unknown addressing type of arg %s\r\n", arg2);
        error_mod();
        return memWord;
    }

    memWord.command.opcode = opcodeIndex;
    
    if(paramsRequired == 2) {
        memWord.command.destAddressing = (addressing_type_arg2 != -1) ? addressing_type_arg2 : 0;
        memWord.command.sourceAddressing = (addressing_type_arg1 != -1) ? addressing_type_arg1 : 0;
        strcpy(memWord.command.sourceValue, arg1);
        strcpy(memWord.command.destValue, arg2);
    } else if(paramsRequired == 1) {
        memWord.command.destAddressing = (addressing_type_arg1 != -1) ? addressing_type_arg1 : 0;
        strcpy(memWord.command.destValue, arg1);
        memWord.command.sourceAddressing = 0;
    } else {
        memWord.command.sourceAddressing = 0;
        memWord.command.destAddressing = 0;
    }

    if(addressing_type_arg1 == ADDRESSING_JUMP_WITH_PARAMS) {
        char jump_arg1[MAX_LINE_LENGTH];
        char jump_arg2[MAX_LABEL_LENGTH];
        char* jmpCommand = arg1;
        int arg1_address_type, arg2_address_type;

        get_arg_of_jump_addressing(jmpCommand, jump_arg1, 1);
        get_arg_of_jump_addressing(jmpCommand, jump_arg2, 2);

        arg1_address_type = get_addressing_type(jump_arg1);
        arg2_address_type = get_addressing_type(jump_arg2);

        memWord.command.param1 = get_param_value_of_address_type(arg1_address_type);
        memWord.command.param2 = get_param_value_of_address_type(arg2_address_type);
    } else {
        memWord.command.param1 = 0;
        memWord.command.param2 = 0;
    }

    return memWord;
}

int get_param_value_of_address_type(int address_type) {
    if(address_type == ADDRESSING_IMMEDIDATELY) {
        return 0;
    } else if(address_type == ADDRESSING_DIRECT_REGISTER) {
        return 3;
    } else if(address_type == ADDRESSING_DIRECTLY) {
        return 1;
    }

    return 0;
}

void set_data_directive(char label_name[MAX_LABEL_LENGTH], char* values) {
    int valueSign = 1;
    char* valuesIter = values;
    int dataValue = 0;
    bool isError = false;
    int dataPointer;
    
    valuesIter = trim(valuesIter);
    valuesIter += DATA_DIRECTIVE_STR_LENGTH; /* Skip .data string */
    valuesIter = trim(valuesIter);

    if(*valuesIter == ',') {
        isError = true;
        printf("[Error] Data directive values cannot start with , before any value\r\n");
        error_mod();
        return;
    }

    dataPointer = dataImagePointer; /* Pointing to the start of label before increasing the pointer of data image */
    while(*valuesIter != '\0') {
        if(isdigit(*valuesIter)) {
            int num = *valuesIter - '0';
            dataValue = dataValue * 10 + num;
            valuesIter++;
        } else if(isspace(*valuesIter) || *valuesIter == '+') {
            valuesIter++;
        } else if(*valuesIter == '-') {
            valueSign = -1;
            valuesIter++;
        } else if(*valuesIter == ',') { /* Setting the value in memory */
            dataImage[dataImagePointer++].number = dataValue * valueSign;
            dataValue = 0;
            valueSign = 1;

            valuesIter++;
            valuesIter = trim(valuesIter);
            if(*valuesIter == ',' || *valuesIter == '\0') { /* In case of , in a row or , before end of line */
                isError = true;
                printf("[Error]: Missed a value of data directive, no value found after ,\r\n");
                error_mod();
                break;
            }
        } else {
            isError = true;
            printf("[Error]: Unknown character '%c' in data directive values\r\n", *valuesIter);
        error_mod();
            break;
        }
    }

    if(!isError) { /* After done, adding the last data value */
        dataImage[dataImagePointer].number = dataValue * valueSign;
        dataImagePointer++;
        /* Saving the label of the data */
        save_label(label_name, dataPointer, data_label);
    }
}

void handle_entry(char label_name[MAX_LABEL_LENGTH], char* values) {
    char* stringIter = values;

    stringIter = trim(stringIter);
    stringIter += ENTRY_DIRECTIVE_STR_LENGTH; /* Skip .entry string */
    stringIter = trim(stringIter);
    
    if(strcmp(label_name, "") != 0) {
        printf("Label name \"%s\" for entry directive is unnecessary.\r\n", label_name);
    }

    remove_new_line(stringIter);

    if(is_valid_label_name(stringIter)) {
        save_label(stringIter, 0, entry_label);
    }
}

void handle_extern(char label_name[MAX_LABEL_LENGTH], char* values) {
    char* stringIter = values;

    stringIter = trim(stringIter);
    stringIter += EXTERN_DIRECTIVE_STR_LENGTH; /* Skip .extern string */
    stringIter = trim(stringIter);
    
    if(strcmp(label_name, "") != 0) {
        printf("Label name \"%s\" for extern directive is unnecessary.\r\n", label_name);
    }

    if(is_valid_label_name(stringIter)) {
        save_label(stringIter, 0, extern_label);
    }
}


void set_string_directive(char label_name[MAX_LABEL_LENGTH], char* values) {
    char* stringIter = values;

    stringIter = trim(stringIter);
    stringIter += STRING_DIRECTIVE_STR_LENGTH; /* Skip .data string */
    stringIter = trim(stringIter);

    if(*stringIter != '"') {
        printf("[Error] Missing string initialize in string directive\r\n");
        error_mod();
        return;
    }
    stringIter++; /* Skipping the first " character */

    save_label(label_name, dataImagePointer, data_label);
    while(*stringIter != '"' && *stringIter != '\0') {
        dataImage[dataImagePointer++].number = *stringIter;
        stringIter++;
    }
    dataImage[dataImagePointer++].number = '\0'; /* Adding End Of String Character */

    if(*stringIter != '"') {
        printf("[Error] Missing end of string character '\"' in string directive\r\n");
        error_mod();
        return;
    }
    stringIter = trim(++stringIter);
    

    if(*stringIter != '\0') {
        printf("[Error] Not valid end of string directive\r\n");
        error_mod();
        return;
    }
}

int opCodeParamsCount(int opcodeIndex) {
    switch(opcodeIndex){
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:
            return 2;
		case 4:
		case 5:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
            return 1;
		case 14:
		case 15:
            return 0;
		default:
            return -1;
	}
}

void process_asm(char* asm_file_name) {
    calculate_labels_addresses(codeImage, codeImagePointer, dataImage, dataImagePointer);
    create_entry_file(asm_file_name);
    /* create_extern_file(asm_file_name, codeImage); */
    create_object_file(asm_file_name, codeImage, codeImagePointer, dataImage, dataImagePointer);
}