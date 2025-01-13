// C allows calling a "undeclared function"--this is a feature
// that stems from the early design decisions and historical
// context of the C programming language. 

// Modern best practices strongly discourage this, and some
// compilers refuse to compile this code. For example, Apple
// clang has -Werror=implicit-function-declaration by default.
//
// #include <stdio.h>
//
// "#include" preprocessor directives is just a copy-paste.
// You can do
// 
//     #include "/dev/urandom"
//
// to stuck some Online Judge.

int main() {
    printf("Hello World\n");
}
