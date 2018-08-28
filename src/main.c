#include <stdio.h>
#include "funcs.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0


/*
 * entry point of assembler program
*/
int main(int argc, char* argv[]) {
    int i;

    if(argc < 2) {
        error("[Error]: missing argument of assembly input files\r\n");
        return EXIT_FAILURE;
    }

    if(processAsmFile(argv[1])) {
        printf("Done proccessing your assembly file.\r\n");
    } else {
        error("[Error]: Problem in your assembly code!\r\n");
    }

    return EXIT_SUCCESS;
}