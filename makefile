preasm: preassembler.c preassembler.h
	gcc -Wall -ansi -pedantic preassembler.c -o ps