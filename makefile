assembler: assembler.o firstpass.o secondpass.o preassembler.o utils.o
	gcc -Wall -ansi -pedantic assembler.o firstpass.o -o assembler -lm
preassembler.o: preassembler.c preassembler.h shared.h utils.h
	gcc -c -Wall -ansi -pedantic preassembler.c -o preassembler.o -lm
firstpass.o: firstpass.c firstpass.h shared.h utils.h
	gcc -c -Wall -ansi -pedantic firstpass.c -o firstpass.o -lm
secondpass.o: secondpass.c secondpass.h shared.h utils.h
	gcc -c -Wall -ansi -pedantic secondpass.c -o secondpass.o -lm
utils.o: utils.c utils.h shared.h utils.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o -lm
assembler.o: assembler.c firstpass.h secondpass.h preassembler.h shared.h utils.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o -lm
