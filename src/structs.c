#include "funcs.h"
#include "assembler.h"
#include "structs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

LabelNode* labelsHead = NULL;
int codeImageFinalAddress = -1; /* After calculated once, it stored here */
int codeAddress[MEMORY_SIZE];
bool input_error = false;

void error_mod() {
    input_error = true;
}

int calculate_code_image_final_address(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer) {
    int i;

    if(codeImageFinalAddress != -1) {
        return codeImageFinalAddress;
    }

    codeImageFinalAddress = START_OF_ADDRESS;
    for(i = 0; i < codeImagePointer; i++) {
        codeImageFinalAddress += count_bytes_of_command(codeImage[i]);
    }

    return codeImageFinalAddress;
}

bool is_has_entry_labels() {
    LabelNode* labelsIterators = labelsHead;
    
    while(labelsIterators != NULL) {
        if(labelsIterators->type == entry_label) {
            return true;
        }
        labelsIterators = labelsIterators->next;
    }

    return false;
}

bool is_has_extern_labels() {
    LabelNode* labelsIterators = labelsHead;
    
    while(labelsIterators != NULL) {
        if(labelsIterators->type == extern_label) {
            return true;
        }
        labelsIterators = labelsIterators->next;
    }

    return false;
}

void create_object_file(char* file_name, MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer, MemoryWord dataImage[MEMORY_SIZE], int dataImagePointer) {
    FILE* object_file;
    char file_path[MAX_FILE_LENGTH];
    int start_of_data_address;
    int i, j;
    char* word_as_bin = (char*)malloc(sizeof(char)*SIZE_OF_BYTE);

    if(input_error) {
        printf("Errors in your asm code, doesn't create object file.\r\n");
        return;
    }
    
    strcpy(file_path, file_name);
    strcat(file_path, ".ob");
    object_file = fopen(file_path, "w+");

    if(object_file == NULL) {
        printf("[Error] Failed to create object file %s\r\n", file_path);
        error_mod();
        return;
    }

    calculate_commands_addresses(codeImage, codeImagePointer);

    for(i = 0; i < codeImagePointer; i++) {
        MemoryWord memWord = codeImage[i];
        int args_amount = opCodeParamsCount(memWord.command.opcode);
        fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i], get_code_bin(memWord));

        if(args_amount == 1) {
            if(memWord.command.destAddressing == ADDRESSING_IMMEDIDATELY || memWord.command.destAddressing == ADDRESSING_DIRECTLY || memWord.command.destAddressing == ADDRESSING_DIRECT_REGISTER) {
                get_arg_bin(memWord.command.destAddressing, memWord.command.destValue, word_as_bin);
                fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+1, word_as_bin);
            } else if(memWord.command.destAddressing == ADDRESSING_JUMP_WITH_PARAMS) {
                char jmp_label_name[MAX_LABEL_LENGTH];
                char* jmpCommand = memWord.command.destValue;
                char jmp_arg1[MAX_LABEL_LENGTH];
                char jmp_arg2[MAX_LABEL_LENGTH];

                get_arg_of_jump_addressing(memWord.command.destValue, jmp_arg1, 1);
                get_arg_of_jump_addressing(memWord.command.destValue, jmp_arg2, 2);

                j = 0;
                while(*(jmpCommand+j) != '(') {
                    jmp_label_name[j] = *(jmpCommand+j);
                    j++;
                }
                jmp_label_name[j] = '\0';
                
                get_label_as_bin(word_as_bin, jmp_label_name);
                
                fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+1, word_as_bin);

                if(memWord.command.param1 == ADDRESSING_DIRECT_REGISTER && memWord.command.param2 == ADDRESSING_DIRECT_REGISTER) {
                    get_bin_of_2regs(word_as_bin,jmp_arg1, jmp_arg2);
                    fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+2, word_as_bin);
                } else {
                    get_arg_bin(memWord.command.param1, jmp_arg1, word_as_bin);
                    fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+2, word_as_bin);
                    get_arg_bin(memWord.command.param2, jmp_arg2, word_as_bin);
                    fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+3, word_as_bin);
                }
            }
        } else if(args_amount == 2) {
            get_arg_bin(memWord.command.sourceAddressing, memWord.command.sourceValue, word_as_bin);
            fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+1, word_as_bin);
            get_arg_bin(memWord.command.destAddressing, memWord.command.destValue, word_as_bin);
            fprintf(object_file, "%d\t\t%s\r\n", codeAddress[i]+2, word_as_bin);
        }
    }

    start_of_data_address = calculate_code_image_final_address(codeImage, codeImagePointer);
    for(i = 0; i < dataImagePointer; i++) {
        num_to_bin_2complete(word_as_bin, dataImage[i].number);
        fprintf(object_file, "%d\t\t%s\r\n", start_of_data_address + i, word_as_bin);
    }

    fclose(object_file);
}

void get_bin_of_2regs(char* dest, char* reg1, char* reg2) {
    char* reg1bin = (char*)malloc(sizeof(char)*7);
    char* reg2bin = (char*)malloc(sizeof(char)*7);
    int reg1num = atoi( reg1 + 1 ); /* +1 Skip char r */
    int reg2num = atoi( reg2 + 1 ); /* +1 Skip char r */

    get_binary_value_of_num(reg1bin, reg1num, 7);
    get_binary_value_of_num(reg2bin, reg2num, 7);

    strcpy(dest, reg2bin);
    strcat(dest, reg1bin);
}

void get_arg_bin(int addressingType, char* arg_value, char* word_as_bin) {
    if(addressingType == ADDRESSING_IMMEDIDATELY) {
        int destNum;
        destNum = atoi(arg_value + 1) /* +1 is skipping the char '#' */;
        num_to_bin_2complete(word_as_bin, destNum);
    } else if(addressingType == ADDRESSING_DIRECTLY) {
        get_label_as_bin(word_as_bin, arg_value);
    } else if(addressingType == ADDRESSING_DIRECT_REGISTER) {
        int destNum = destNum = atoi(arg_value + 1) /* +1 is skipping the char 'r' */;
        num_to_bin_2complete(word_as_bin, destNum);
    }
}

void get_label_as_bin(char* dest, char* labelName) {
    LabelNode* destLabel;
    int labelAddress;
    
    destLabel = find_local_label_by_name(labelName);
    if(destLabel == NULL) {
        int i;
        destLabel = get_label_by_name(labelName);
        if(destLabel == NULL) {
            for(i = 0; i < SIZE_OF_BYTE; i++) {
                *(dest+i) = ASM_VALUE_OF_0;
            }
        } else if(destLabel->type == extern_label) {
            for(i = 0; i < SIZE_OF_BYTE-2; i++) {
                *(dest+i) = ASM_VALUE_OF_0;
            }
            *(dest+SIZE_OF_BYTE-2) = ASM_VALUE_OF_0;
            *(dest+SIZE_OF_BYTE-1) = ASM_VALUE_OF_1;
        } else { /* In case of entry or unknwon */
            for(i = 0; i < SIZE_OF_BYTE; i++) {
                *(dest+i) = ASM_VALUE_OF_0;
            }
        }
    } else {
        char* temp = (char*)malloc(sizeof(char)*SIZE_OF_BYTE-2);
        labelAddress = destLabel->address;
        get_binary_value_of_num(temp, labelAddress, SIZE_OF_BYTE-2);
        strcpy(dest, temp);
        /* This label is relocatable */
        *(dest+SIZE_OF_BYTE-2) = ASM_VALUE_OF_1;
        *(dest+SIZE_OF_BYTE-1) = ASM_VALUE_OF_0;
    }
}

void num_to_bin_2complete(char* dest, int num) {
    int i;
    int sign = 1;

    for(i=0; i < SIZE_OF_BYTE; i++) {
        *(dest+i) = ASM_VALUE_OF_0;
    }

    if(num < 0) {
        sign = -1;
        num *= -1;
    }

    get_binary_value_of_num(dest, num, SIZE_OF_BYTE);
    
    if(sign < 0) {
        /* Reverse the bytes and add 1 */
        *dest = ASM_VALUE_OF_1;
        for(i = 1; i < SIZE_OF_BYTE; i++) {
            if(*(dest+i) == ASM_VALUE_OF_0) {
                *(dest+i) = ASM_VALUE_OF_1;
            } else {
                *(dest+i) = ASM_VALUE_OF_0;
            }
        }
    }
}

char* get_code_bin(MemoryWord word) {
    char* code = (char*)malloc(sizeof(char)*SIZE_OF_BYTE);
    char* bin_value_2chars = (char*)malloc(sizeof(char) * 2 + 1);
    char* bin_value_4chars = (char*)malloc(sizeof(char) * 4 + 1);

    /* Creating string of bin : left to right */
    get_binary_value_of_num(bin_value_2chars, word.command.param1, 2);
    strcpy(code, bin_value_2chars);
    get_binary_value_of_num(bin_value_2chars, word.command.param2, 2);
    strcat(code, bin_value_2chars);
    get_binary_value_of_num(bin_value_4chars, word.command.opcode, 4);
    strcat(code, bin_value_4chars);
    get_binary_value_of_num(bin_value_2chars, word.command.sourceAddressing, 2);
    strcat(code, bin_value_2chars);
    get_binary_value_of_num(bin_value_2chars, word.command.destAddressing, 2);
    strcat(code, bin_value_2chars);
    get_binary_value_of_num(bin_value_2chars, 0, 2);
    strcat(code, bin_value_2chars);

    return code;
}

char* get_binary_value_of_num(char* dest, int num, int chars) {
    int i, digitsCounter;
    char* bin_value = dest;

    /* Reset bin values */
    for(i = 0; i < chars; i++) {
        *(bin_value+i) = ASM_VALUE_OF_0;
    }

    digitsCounter = 0;
    while(num > 0) {
        *(bin_value + chars - digitsCounter - 1) = (num % 2 == 0) ? ASM_VALUE_OF_0 : ASM_VALUE_OF_1;
        digitsCounter++;
        num = num / 2;
    }
    *(bin_value + chars) = '\0';

    return bin_value;
}

void calculate_commands_addresses(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer) {
    int i;
    int address_value = START_OF_ADDRESS;

    for(i = 0; i < codeImagePointer; i++) {
        codeAddress[i] = address_value;
        address_value += count_bytes_of_command(codeImage[i]);
    }
}

void create_entry_file(char* file_name) {
    LabelNode* labelsIterators = labelsHead;
    FILE* entry_file;
    char file_path[MAX_FILE_LENGTH];

    if(input_error) {
        printf("Errors in your asm code, doesn't create entry file.\r\n");
        return;
    }

    if(!is_has_entry_labels()) {
        return; /* There's no entry file, no use for continue */
    }
    
    strcpy(file_path, file_name);
    strcat(file_path, ".ent");
    entry_file = fopen(file_path, "w+");

    if(entry_file == NULL) {
        printf("[Error] Cannot write/create the file \"%s\"\r\n", file_path);
        return;
    }

    while(labelsIterators != NULL) {
        if(labelsIterators->type == entry_label) {
            LabelNode* targetLabel = find_local_label_by_name(labelsIterators->name);
            if(targetLabel == NULL) {
                printf("[Error] couldn't file label \"%s\" so cannot entry it.\r\n", labelsIterators->name);
            } else {
                fprintf(entry_file, "%s\t\t%d\r\n", targetLabel->name, targetLabel->address);
            }
        }
        labelsIterators = labelsIterators->next;
    }

    fclose(entry_file);
}

void create_extern_file(char* file_name, MemoryWord codeImage[MEMORY_SIZE]) {
    LabelNode* labelsIterators = labelsHead;
    FILE* extern_file;
    char file_path[MAX_FILE_LENGTH];


    if(input_error) {
        printf("Errors in your asm code, doesn't create extern file.\r\n");
        return;
    }

    if(!is_has_extern_labels()) {
        return; /* There's no externy directive, no use for continue */
    }

    strcpy(file_path, file_name);
    strcat(file_path, ".ext");
    extern_file = fopen(file_path, "w+");

    if(extern_file == NULL) {
        printf("[Error] Cannot write/create file \"%s\"\r\n", file_path);
        error_mod();
    }

    while(labelsIterators != NULL) {
        if(labelsIterators->type == extern_label) {
            
        }
        labelsIterators = labelsIterators->next;
    }

    fclose(extern_file); 
}


bool is_label_exists(char* name) {
    LabelNode* labelsIterators;

    if(strcmp(name, "") == 0) {
        return false;
    }
    
    labelsIterators = labelsHead;
    while(labelsIterators != NULL) {
        if(strcmp(name, labelsIterators->name) == 0 && labelsIterators->type != entry_label && labelsIterators->type != extern_label) {
            return true;
        }
        labelsIterators = labelsIterators->next;
    }

    return false;
}

LabelNode* find_local_label_by_name(char* name) {
    LabelNode* labelsIterators = labelsHead;

    while(labelsIterators != NULL) {
        if(strcmp(labelsIterators->name, name) == 0 && labelsIterators->type != entry_label && labelsIterators->type != extern_label) {
            return labelsIterators;
        }
        labelsIterators = labelsIterators->next;
    }

    return NULL;
}

void save_label(char* name, int memoryPointer, enum LabelType type) {
    LabelNode* newLabel = (LabelNode*)malloc(sizeof(LabelNode));
    LabelNode* labelsIterators;
    char labelName[MAX_LABEL_LENGTH];

    name = trim(name);
    if(strcmp(name, "") == 0) { /* Not saving a null label */
        return;
    }

    remove_spaces(name, labelName);
    strcpy(newLabel->name, labelName);
    newLabel->type = type;
    newLabel->memoryPointer = memoryPointer;
    newLabel->next = NULL;
    newLabel->address = 0; /* Will be calculated later */

    if(labelsHead == NULL) {
        labelsHead = newLabel;
        return;
    }

    labelsIterators = labelsHead;
    while(labelsIterators->next != NULL) {
        labelsIterators = labelsIterators->next;
    }

    labelsIterators->next = newLabel;
}

LabelNode* get_label_by_name(char* name) {
    LabelNode* labelsIterators;

    if(strcmp(name, "") == 0) {
        return NULL;
    }
    
    labelsIterators = labelsHead;
    while(labelsIterators != NULL) {
        if(strcmp(name, labelsIterators->name) == 0) {
            return labelsIterators;
        }
        labelsIterators = labelsIterators->next;
    }

    return NULL;
}

void calculate_labels_addresses(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer, MemoryWord dataImage[MEMORY_SIZE], int dataImagePointer) {
    LabelNode* labelsIterators = labelsHead;
    int i, addressValue;
    
    while(labelsIterators != NULL) {
        if(labelsIterators->type == code_label) {
            /* Calculate the address of label which is code */
            addressValue = START_OF_ADDRESS;
            for(i = 0; i < labelsIterators->memoryPointer; i++) {
                addressValue += count_bytes_of_command(codeImage[i]);
            }
        } else if(labelsIterators->type == data_label) {
            /* Calculate the address of label which is data */
            addressValue = calculate_code_image_final_address(codeImage, codeImagePointer);
            addressValue += labelsIterators->memoryPointer; /* Every memory pointer in data code is 1 byte */
        }

        labelsIterators->address = addressValue;
        labelsIterators = labelsIterators->next;
    }
}

int count_bytes_of_command(MemoryWord memoryCode) {
    int counter = 1;/* Because of command itself is a byte */
    int paramsAmount = opCodeParamsCount(memoryCode.command.opcode);

    if(paramsAmount == 1) {
        if(memoryCode.command.destAddressing == ADDRESSING_JUMP_WITH_PARAMS) {
            counter += 1; /* Jump has label name which is 1 byte */
            if(memoryCode.command.param1 == ADDRESSING_DIRECT_REGISTER && memoryCode.command.param2 == ADDRESSING_DIRECT_REGISTER) {
                counter += 1; /* Because 2 register displayed in 1 byte */
            } else {
                counter += 2; /* 1 byte per arg (2 args total) */
            }
        } else { /* In any case it's not jump addressing, this param will take only 1 byte */
            counter += 1; 
        }
    } else if(paramsAmount == 2) {
        if(memoryCode.command.sourceAddressing == ADDRESSING_DIRECT_REGISTER && memoryCode.command.destAddressing == ADDRESSING_DIRECT_REGISTER) {
            counter += 1; /* two registers is set in 1 byte */
        } else {
            counter += 2;
        }
    }

    return counter;
}
