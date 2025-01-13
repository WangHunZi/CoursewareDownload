// Copyright 2024 Hanzhi Liu (MIT Licenced)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mini-rv32ima.h"

#define RAM_SIZE (64*1024*1024) // Just default RAM amount is 64MB.
#define RAM_TEXT_START   0
#define RAM_TEXT_END     RAM_STACK_START
#define RAM_STACK_START (RAM_SIZE/2)
#define RAM_STACK_END    RAM_SIZE

void DumpState(struct CPUState * core) {
	uint32_t pc = core->csrs[PC];
	uint32_t pc_offset = pc - RAM_TEXT_START;
	uint32_t ir = 0;
	printf(" PC:%08x", pc);
	if (pc_offset >= 0 && pc_offset < RAM_TEXT_END - 3) {
		ir = *((uint32_t*)(&((uint8_t*)core->mem)[pc_offset]));
		printf(" [%08x]", ir);
	} else {
		printf(" [xxxxxxxx]");
    }
    printf("\n");
    uint32_t * regs = core->regs;
	printf("  Z:%08x  ra:%08x  sp:%08x  gp:%08x\n",  regs[Z], regs[RA], regs[SP], regs[GP]);
    printf(" tp:%08x  t0:%08x  t1:%08x  t2:%08x\n", regs[TP], regs[T0], regs[T1], regs[T2]);
    printf(" s0:%08x  s1:%08x  a0:%08x  a1:%08x\n", regs[S0], regs[S1], regs[A0], regs[A1]);
    printf(" a2:%08x  a3:%08x  a4:%08x  a5:%08x\n", regs[A2], regs[A3], regs[A4], regs[A5]);
	printf(" a6:%08x  a7:%08x  s2:%08x  s3:%08x\n", regs[A6], regs[A7], regs[S2], regs[S3]);
    printf(" s4:%08x  s5:%08x  s6:%08x  s7:%08x\n", regs[S4], regs[S5], regs[S6], regs[S7]);
    printf(" s8:%08x  s9:%08x s10:%08x s11:%08x\n", regs[S8], regs[S9], regs[S10], regs[S11]);
    printf(" t3:%08x  t4:%08x  t5:%08x  t6:%08x\n", regs[T3], regs[T4], regs[T5], regs[T6]);
	uint32_t * csrs = core->csrs;
    printf("CYCLEL:%08x CYCLEH:%08x EXTRAFLAGS:%08x\n", csrs[CYCLEL], csrs[CYCLEH], csrs[EXTRAFLAGS]);
    printf("stack (sp:%08x):\n", regs[2]);
    for (int sp = regs[2]; sp < core->mem_size; sp += sizeof(int32_t)) {
        printf("%08x: %08x\n", sp, core->mem[sp]);
    }
    printf("%#08x\n", RAM_STACK_END);
    printf("\n");
}
int xtoi(char * s) {
    int res = 0;
    for (int i = 2; s[i]; i ++) {
        res *= 16;
        if (s[i] >= 'a' && s[i] <= 'f') {
            res =+ s[i] - 'a';
        } else {
            res += s[i] - '0';
        }
    }
    return res;
}

int main(int argc, char ** argv) {
    // get the testcase
    if (argc < 2) {
        printf("Usage: ./mini-rv32ima <path_testcase> <arg1> <arg2> ... <argn>\n");
        printf("- The testcase file should be a rv32i binary with 0 offset to the first line of instruction.\n");
        printf("- Note that we only support dec/hex int-type mainargs for simplicity.\n");
        return 0;
    }
    char * image_filename = argv[1];
    printf("[mini-rv32ima] load image file: %s\n", image_filename);

    // allocate ram image
    struct CPUState state;
    printf("[mini-rv32ima] alloc ram size = %#x\n", RAM_SIZE);
    memset(&state, 0, sizeof(state));
    state.mem = malloc(RAM_SIZE);
    if (!state.mem) {
		fprintf(stderr, "Error: failed to allocate ram image.\n");
		return 1;
	}
    memset(state.mem, 0, state.mem_size);
    state.mem_offset = RAM_TEXT_START;
    state.mem_size = RAM_SIZE;
	state.csrs[PC] = state.mem_offset;

    // load insts from testcase
    FILE * f = fopen(image_filename, "rb");
    if (!f || ferror(f)) {
        fprintf(stderr, "Error: image file \"%s\" not found\n", image_filename);
        return 1;
	}
    fseek(f, 0, SEEK_END);
    long flen = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (flen > RAM_TEXT_END - RAM_TEXT_START) {
        fprintf(stderr, "Error: image file size too big (%#lx bytes)\n", flen);
        fclose(f);
        return 1;
    }
    if (fread(state.mem + RAM_TEXT_START, flen, 1, f) != 1) {
        fprintf(stderr, "Error: Failed to load image file\n");
        return 1;
    }
    fclose(f);


    // get mainargs
    #define MAX_MAINARGS 4
    if (argc > 2 + MAX_MAINARGS) {
        printf("[mini-rv32ima] WARN: mainargs should <= %d, the excess ones will be discarded\n", MAX_MAINARGS);
    }
    int margc = (argc > 2 + MAX_MAINARGS ? MAX_MAINARGS : argc - 2);
    int margs[MAX_MAINARGS] = {0};
    printf("[mini-rv32ima] mainargs:\n");
    for (int i = 0; i < margc; i ++) {
        if (argv[2 + i][0] == '0' && argv[2 + i][1] == 'x') {
            margs[i] = xtoi(argv[2 + i]);
        } else {
            margs[i] = atoi(argv[2 + i]);
        }
        printf("- %d\n", margs[i]);
    }
    uint32_t sp = RAM_STACK_END - margc * sizeof(int32_t);
    memcpy(state.mem + sp, margs, RAM_STACK_END - sp);
	state.regs[SP] = sp;
	state.regs[A0] = margc;
	state.regs[A1] = sp;

    // do emulation
    printf("initially:\n");
    DumpState(&state);
    int ret;
    int debug_climit = 10000;
    do {
        ret = rv32ima_step(&state, 1);
        if (ret != 0) printf("minirv32ima ret=%d !=0\n", ret);
        // DumpState(&state);
        if (--debug_climit <= 0) {
            fprintf(stderr, "Error: debug_climit exceed\n");
            break;
        }
    } while (ret == 0 && state.csrs[PC] != 0);
    printf("finally:\n");
    DumpState(&state);
    // return
    return 0;
}