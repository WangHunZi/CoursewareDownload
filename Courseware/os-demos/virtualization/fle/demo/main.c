#include "minilib.h"

// Global data
int n = 10;

// External data
extern char msg[];

// External function
int foo();

int main() {
    // Pointers
    const char *p = msg;

    // Function calls
    for (int i = 0; i < foo(); i++) {
        print("Message: ", p, NULL);
    }

    return 42;
}
