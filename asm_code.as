TEST:   mov       r4,         #-1
  not     r3       
    HELLO1: .data +3,   -1 , 3, +9
  WORLD4: .string "Hello World"
LOOP: mov #-1, r3
dec r2
inc r5
TEST2:   mov       x,         r3
  not     r3       
    HELLO: .data +3,   -1 , 3, +9
  WORLD: .string "Hello World"
LOOP1: mov #-1, r3
dec r2
inc r5

jsr TEST3(r2 , #-1)
LOOP3: mov #-1, r3
.entry TEST2
.extern TEST2