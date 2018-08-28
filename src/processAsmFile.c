#include "funcs.h"
#include "assembler.h"
#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool processAsmFile(char* asm_file_name) {
    FILE* asm_file;
    char* asm_file_path;
    char asm_line[MAX_LINE_LENGTH];
    char errorMessage[STRING_LENGTH];

    asm_file_path = (char*)malloc(sizeof(asm_file_name)+sizeof(char)*4); /* 3 chars for .as\0 file */
    strcpy(asm_file_path, asm_file_name);
    strcat(asm_file_path, ".as");

    asm_file = fopen(asm_file_path, "r");

    if(!asm_file) {
        printf("[Error] Cannot read file: \"%s\"\r\n", asm_file_path);
        error_mod();
        return false;
    }
    
    while(fgets(asm_line, MAX_LINE_LENGTH, asm_file) != NULL) {
        char label_name[MAX_LABEL_LENGTH];
        
        if(is_comment(asm_line) || is_empty(asm_line)) {
            continue;
        }

        get_label_name(label_name, asm_line);

        if(!is_valid_label_name(label_name)) {
            scanf(errorMessage, "Error: invalid label name \"%s\"", label_name);
            error(errorMessage);
            continue;
        }

        if(is_label_exists(label_name)) {
            printf("Label \"%s\" is already declared\r\n", label_name);
            continue;
        }
        
        if(is_data_directive(asm_line)) {
            /* Skipping to directive values */
            char* values = asm_line;
            while(*values != '.') {
                values++;
            }

            set_data_directive(label_name, values);
        } else if(is_string_directive(asm_line)) {
            /* Skipping to directive values */
            char* values = asm_line;
            while(*values != '.') {
                values++;
            }

            set_string_directive(label_name, values);
        } else if(is_entry_directive(asm_line)) {
            /* Skipping to directive values */
            char* values = asm_line;
            while(*values != '.') {
                values++;
            }

            handle_entry(label_name, values);
        } else if(is_extern_directive(asm_line)) {
            /* Skipping to directive values */
            char* values = asm_line;
            while(*values != '.') {
                values++;
            }

            handle_extern(label_name, values);
        } else { /* In this case, it should be a command */
            char* command;
            if(strlen(label_name) > 0) {
                command = asm_line + strlen(label_name) + 1; /* +1 because of the char : */
            } else {
                command = asm_line;
            }
            
            command = trim(command);
            handle_command(label_name, command);
        }
    }

    fclose(asm_file);

    process_asm(asm_file_name);

    return true;
}
