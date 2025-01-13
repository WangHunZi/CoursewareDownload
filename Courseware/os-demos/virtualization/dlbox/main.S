#include "dl.h"

DL_HEAD

LOAD("libc.dl")
LOAD("libhello.dl")
IMPORT(hello)
IMPORT(exit)
EXPORT(_start)

DL_CODE

main:
    call DSYM(hello)
    call DSYM(hello)
    call DSYM(hello)
    call DSYM(hello)
    movq $0, %rax
    ret

_start:
    call main
    jmp DSYM(exit)

DL_END
