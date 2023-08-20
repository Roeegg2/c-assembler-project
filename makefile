assembler: assembler.o firstpass.o secondpass.o preassembler.o utils.o
	gcc -g -Wall -ansi -pedantic assembler.o firstpass.o secondpass.o preassembler.o utils.o -lm -o assembler

preassembler.o: preassembler.c preassembler.h shared.h utils.h
	gcc -c -g -Wall -ansi -pedantic preassembler.c -o preassembler.o

firstpass.o: firstpass.c firstpass.h shared.h utils.h
	gcc -c -g -Wall -ansi -pedantic firstpass.c -o firstpass.o

secondpass.o: secondpass.c secondpass.h shared.h utils.h
	gcc -c -g -Wall -ansi -pedantic secondpass.c -o secondpass.o

utils.o: utils.c shared.h utils.h
	gcc -c -g -Wall -ansi -pedantic utils.c -o utils.o

assembler.o: assembler.c firstpass.h secondpass.h preassembler.h shared.h utils.h
	gcc -c -g -Wall -ansi -pedantic assembler.c -o assembler.o

