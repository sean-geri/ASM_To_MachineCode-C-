MAIN: mov r3 ,LENGTH
LOOP: jmp L1(#-1,r6) 
prn #-5 
bne LOOP(r4,r5)
sub r1, r4 
bne END 
L1: inc K 
bne LOOP(K,STR) 
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15 
K: .data 22