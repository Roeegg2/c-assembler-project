MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
mcro m1


sub @r1, @r4

bne END

endmcro

mcro this
thiscode!!!
endmcro
prn -5
bne LOOP
m1
L1: inc K
bne LOOP
END: stop
STR: .string “abcdef”
this
LENGTH: .data 6,-9,15
this
K: .data 22