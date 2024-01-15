The instructions regarding the project had some unaddresses issues and conflicts. In such cases, I followed what seemed most reasonable and what most of the other students/proffessors said. I would like to clarify some of these things:

1. There was a conflict regarding the marking of multiple labels as extern/entry in the same line (for example .extern L1, L2, L3). I made it possible to mark multiple labels in the same line (both .extern and .entry)
2. There was a conflict regarding the duplicate marking of labels. In case the 2 markings are of the same type, I will only print a warning (for example on line 1: .extern L1 and on line 2: .extern L1) but if the markings are of a different type (same label marked both as .extern and .entry) I will return an error.
3. I made sure both macros and labels arent named a saved word, and reported an error accordingly
4. A line is considered a comment only if and only if the first character in that line ';'
5. In case of allocation error and an error in opening a file I exited the program (except if the file is an .as file, and then I moved printed a message to the console and moved to the next file). If the file was either not found, the file was blank or the file didnt contain any code (meaning the file only contained white notes and comments) I printed a message accordingly and moved on to the next file.
6. There were many different answers regarding this, and so I followed what most said: If any errors are found on preassembler stage the assembler wont move on to the firstpass and secondpass (and the same thing with firstpass- if any error is found on firstpass the assembler wont continue to the secondpass).

Additionally to that, I would like to explain the structure of the files in the assembler:
shared.h --> contains macros and includes which all the c files use 
passes_utils.h --> utility functions used by only firstpass, preassembler and secondpass (not assembler.c) 
shared_fp_pa, shared_fp_sp --> each contain macros, functions, and structs which are shared with firstpass, and preassembler and secondpass respectively 
preassembler.h, firstpass.h secondpass.h --> each contain macros, functions and structs which are specifically and only used in preassembler, firstpass, and second pass respectively 
assembler.h --> contains firstpass.h, secondpass.h, preassembler.h

Thank you for reading. If you have any insights or notes regarding my project (even outside of this course's curiculum), I would be very happy to hear them. I want to improve both my C knowledge and project management, and so any insight/note would be very appriciated.

Name: Roee Toledano
