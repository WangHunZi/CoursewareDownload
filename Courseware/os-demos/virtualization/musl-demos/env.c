#include <stdio.h>

// A mysteriously defined symbol.
// Someone must defined it elsewhere.
extern char **environ;

// Like this even more mysterious one.
// "end" can be of any type.
extern void ******************************end;

int main() {
    for (char **env = environ; *env; env++) {
        // key=value
        printf("%s\n", *env);
    }

    end = NULL; // ???
}
