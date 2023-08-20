assembler: assembler.o firstpass.o secondpass.o preassembler.o passes_utils.o
	gcc -g -Wall -ansi -pedantic assembler.o firstpass.o secondpass.o preassembler.o passes_utils.o -lm -o assembler

preassembler.o: preassembler.c shared.h preassembler.h shared_fp_pa.h passes_utils.h
	gcc -c -g -Wall -ansi -pedantic preassembler.c -o preassembler.o

firstpass.o: firstpass.c shared.h firstpass.h shared_fp_pa.h shared_fp_sp.h passes_utils.h
	gcc -c -g -Wall -ansi -pedantic firstpass.c -o firstpass.o

secondpass.o: secondpass.c shared.h secondpass.h shared_fp_sp.h passes_utils.h
	gcc -c -g -Wall -ansi -pedantic secondpass.c -o secondpass.o

passes_utils.o: passes_utils.c shared.h passes_utils.h
	gcc -c -g -Wall -ansi -pedantic passes_utils.c -o passes_utils.o

assembler.o: assembler.c assembler.h firstpass.h secondpass.h preassembler.h shared.h
	gcc -c -g -Wall -ansi -pedantic assembler.c -o assembler.o

