#ifndef STRUCT_H
#define STRUCT_H

enum LabelType {
    code_label = 0,
    data_label = 1,
    entry_label = 2,
    extern_label = 3
};

struct LabelNode_ {
    int memoryPointer;
    char name[MAX_LABEL_LENGTH];
    enum LabelType type;
    struct LabelNode_* next;
    int address;
};

typedef struct LabelNode_ LabelNode; /* Define LabelNode to be struct LabelNode_ */

void error_mod();
bool is_label_exists(char* labelName);
void save_label(char* name, int memoryPointer, enum LabelType type);
void create_entry_file(char* file_name);
void create_extern_file(char* file_name, MemoryWord codeImage[MEMORY_SIZE]);
bool is_has_extern_labels();
void create_object_file(char* file_name, MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer, MemoryWord dataImage[MEMORY_SIZE], int dataImagePointer);
LabelNode* find_local_label_by_name(char* name);
bool is_has_entry_labels();
LabelNode* get_label_by_name(char* name);
char* get_code_bin(MemoryWord word);
void calculate_labels_addresses(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer, MemoryWord dataImage[MEMORY_SIZE], int dataImagePointer);
int count_bytes_of_command(MemoryWord memoryCode);
int calculate_code_image_final_address(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer);
void calculate_commands_addresses(MemoryWord codeImage[MEMORY_SIZE], int codeImagePointer);
char* get_binary_value_of_num(char* dest, int num, int chars);
void num_to_bin_2complete(char* dest, int num);
void get_label_as_bin(char* dest, char* labelName);
void get_arg_bin(int addressingType, char* arg_value, char* word_as_bin);
void get_bin_of_2regs(char* dest, char* reg1, char* reg2);

#endif