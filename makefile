COMPILE_COMMAND = gcc -Wall -ansi -pedantic
MAKE_BIN_DIR = mkdir -p bin
ALLOW_EXEC = chmod +x


assembler: ./src/main.c ./src/processAsmFile.c ./src/structs.c ./src/funcs.c ./src/assembler.c ./src/assembler.h
	$(MAKE_BIN_DIR)
	$(COMPILE_COMMAND) -c ./src/main.c -o ./bin/main
	$(COMPILE_COMMAND) -c ./src/processAsmFile.c -o ./bin/processAsmFile
	$(COMPILE_COMMAND) -c ./src/assembler.c -o ./bin/assembler
	$(COMPILE_COMMAND) -c ./src/funcs.c -o ./bin/funcs
	$(COMPILE_COMMAND) -c ./src/structs.c -o ./bin/structs
	$(COMPILE_COMMAND) -g ./bin/main ./bin/processAsmFile ./bin/structs ./bin/assembler ./bin/funcs -o ./assembler
	$(ALLOW_EXEC) ./assembler