#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "funcs.h"

char* opcodeStrings[] = { "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
char* registersString[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };
char* directivesStrings[] = { "data", "string", "entry", "extern" };

bool is_comment(char line[MAX_LINE_LENGTH]) {
    if(line[0] == ';') {
        return true;
    }

    return false;
}

bool is_empty(char line[MAX_LINE_LENGTH]) {
    int i;
    for(i = 0; i < strlen(line); i++) {
        if(!isspace(line[i]))
            return false;
    }

    return true;
}

bool is_data_directive(char line[MAX_LINE_LENGTH]) {
    return strstr(line, ".data") != NULL;
}

bool is_entry_directive(char line[MAX_LINE_LENGTH]) {
    return strstr(line, ".entry") != NULL;
}

bool is_string_directive(char line[MAX_LINE_LENGTH]) {
    return strstr(line, ".string") != NULL;
}

bool is_extern_directive(char line[MAX_LINE_LENGTH]) {
    return strstr(line, ".extern") != NULL;
}

void get_opcode_from_command(char* command, char* opcodeDest) {
    int counter = 0;
    char* temp = command;
    
    while(!isspace(*temp) && *temp != ',' && *temp != '\0' && counter < MAX_OPCODE_LENGTH) {
        *(opcodeDest++) = *(temp++);
        counter++;
    }
    *opcodeDest = '\0';
}

void get_value_from_args(char* valDest, char* args, int arg_num) {
    char* temp = args;
    int args_counter = 0;
    bool is_on_bracket = false;

    while(*temp != '\0') {
        if(args_counter == arg_num - 1 && (*temp != ',' || is_on_bracket) && !isspace(*temp)) {
            *(valDest++) = *(temp++);
        } else {
            temp++;
        }

        if(*temp == ',' && !is_on_bracket)
            args_counter++;
        if(*temp == '(')
            is_on_bracket = true;
        if(*temp == ')')
            is_on_bracket = false;
    }

    *valDest = '\0'; /* End of string for argument */
}

void remove_new_line(char* str) {
    char* temp = str;
    while(*temp != '\0') {
        if(*temp == '\n') {
            *temp = '\0';
        }
        temp++;
    }
}

bool is_immedidately_addressing(char* arg) {
    char* temp = arg;
    int length, i;

    if(*temp != '#') {
        return false;
    }

    temp++; /* Skipping the char '#' */
    if(!isdigit(*temp) && *temp != '-' && *temp != '+') {
        return false;
    }
    temp++; /* Skipping the checked char */

    length = strlen(temp);
    for(i = 0; i < length; i++, temp++) {
        if(!isdigit(*temp)) {
            return false;
        }
    }

    return true;
}

bool is_jump_addressing(char* arg) {
    char* tmp = arg;
    char arg1[MAX_LABEL_LENGTH], arg2[MAX_LABEL_LENGTH];
    char inside_brackets[MAX_LINE_LENGTH];
    
    if(strchr(tmp, '(') == NULL || strchr(tmp, ')') == NULL) {
        return false;
    }

    get_inside_brackets(tmp, inside_brackets);

    get_value_from_args(arg1, inside_brackets, 1);
    get_value_from_args(arg2, inside_brackets, 2);

    if(
        get_addressing_type(arg1) == ADDRESSING_JUMP_WITH_PARAMS ||
        get_addressing_type(arg2) == ADDRESSING_JUMP_WITH_PARAMS || 
        get_addressing_type(arg1) == -1 ||
        get_addressing_type(arg2) == -1
    ) {
        /* Args of jump addresing cannot be jump addressing type or unknown addressing type */
        /* TODO: throw error */
        return false;
    }

    return true;
}

void get_arg_of_jump_addressing(char* jmpCommand, char* argDest, int arg_num) {
    char inside_brackets[MAX_LINE_LENGTH];
    
    get_inside_brackets(jmpCommand, inside_brackets);

    get_value_from_args(argDest, inside_brackets, arg_num);
}

void get_inside_brackets(char* str, char* destValue) {
    char* tmp = str;

    if(strchr(tmp, '(') == NULL || strchr(tmp, ')') == NULL) {
        *destValue = '\0';
        return;
    }

    while(*tmp != '(') {
        if(*tmp == ')') { /* If brackets closed before opened...! */
            *destValue = '\0';
            return;
        }
        tmp++;
    }
    tmp++; /* Skipp the char '(' */

    while(*tmp != ')') {
        *(destValue++) = *(tmp++);
    }
    *destValue = '\0'; /* Replace '(' in end of str */
}

bool is_directly_addressing(char* arg) {
    char* op = arg;
	if(!(isalpha(*op)))
		return false;
    
	while(isalnum(*op)) {
		op++;   
    }
	op = trim(op);

	if(*op != ',' && *op != '\0')
		return false;
    
	return true;
}

bool is_direct_register_addressing(char* arg) {
    char* t = arg;
    int register_num;

    if(*t == 'r' && isdigit(*(t+1))) {
        t++; /* skip 'r' char */
        register_num = atoi(t);

        if(register_num <= 7) {
            return true;
        }
    }
    
    return false;
}

char *strrev(char *str) {
    char* p1;
    char* p2;

    if(!str || !*str)
        return str;
    
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }

    return str;
}

int get_addressing_type(char* arg) {
	if(is_immedidately_addressing(arg)) {
        return ADDRESSING_IMMEDIDATELY;
    } else if(is_direct_register_addressing(arg)) {
        return ADDRESSING_DIRECT_REGISTER;
    } else if(is_directly_addressing(arg)) {
        return ADDRESSING_DIRECTLY;
    } else if (is_jump_addressing(arg)) {
        return ADDRESSING_JUMP_WITH_PARAMS;
    } else {
        return -1;
    }
}

void error(char* err) {
    printf("[Error] %s", err);
}

void remove_spaces(char* input, char* result) {
    int i, j = 0;
    for(i = 0; input[i] != '\0'; i++) {
        if(!isspace((unsigned char) input[i])) {
            result[j++] = input[i];
        }
    }

    result[j] = '\0';
}

char* trim(char* str) {
    char* t = str;
    while(isspace(*t)) {
        t++;
    }
    return t;
}


void get_label_name(char* label, char* asm_line) {
    if(strchr(asm_line, ':') == NULL) {
        *label = '\0';
    } else {
        char* temp_label = label;
        char* temp_asm_line = asm_line;
        temp_asm_line = trim(temp_asm_line);

        while(*temp_asm_line != ':') {
            *temp_label++ = *temp_asm_line++;
        }
        *temp_label = '\0';
    }
}

bool is_valid_label_name(char* label_name) {
    int i;
    
    label_name = trim(label_name);

    if(strcmp("", label_name) == 0) { /* Not every line must have label name */
        return true;
    }

    if(!isalpha(*label_name)) {
        printf("[Error]: Label name \"%s\" is not valid because label first char must be alphabet.\r\n", label_name);
        return false;
    }

    for(i = 0; i < OPCODE_AMOUNT; i++) {
        if(strcmp(opcodeStrings[i], label_name) == 0) {
            printf("[Error] Label name \"%s\" is not valid because it's using saved word in assembly.\r\n", label_name);
            return false;
        }
    }

    for(i = 0; i < REGISTERS_AMOUNT; i++) {
        if(strcmp(registersString[i], label_name) == 0) {
            printf("[Error] Label name \"%s\" is not valid because it's using saved word in assembly.\r\n", label_name);
            return false;
        }
    }

    for(i = 0; i < DIRECTIVES_AMOUNT; i++) {
        if(strcmp(directivesStrings[i], label_name) == 0) {
            printf("[Error] Label name \"%s\" is not valid because it's using saved word in assembly.\r\n", label_name);
            return false;
        }
    }

    return true;
}

