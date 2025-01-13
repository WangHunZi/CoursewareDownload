minimal: minimal.S

# Although gcc is most commonly associated with C and C++,
# it can also be used for assembly language files.
#     -S: Compile the input file but stop after the compilation
#      phase; do not assemble
#     .S files: By convention, assembly source files often
#      have the extension `.S` or `.s`. The difference is that
#      files with `.S` are preprocessed by the C preprocessor,
#      allowing for macros and include files, while `.s` files
#      are not.
# The outputs the output of the compilation (assembly code)
# is redirected to "minimal.s", indicating that this file is
# not intended to be preprocessed further.
	gcc -g -S minimal.S > minimal.s

# as is the GNU assembler. It takes assembly code as input
# and produces object code. Object code is machine code that
# is not yet fully linked into an executable; it may contain
# placeholders for addresses that are only resolved at link time.
	as minimal.s -o minimal.o

# ld is the GNU linker. It takes one or more object files and
# combine them into a single executable file, resolving any
# references between them (such as function calls) in the process.
	ld -o minimal minimal.o

clean:
	rm -f minimal.s minimal.o minimal
