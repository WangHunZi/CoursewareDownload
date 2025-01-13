#include <stdio.h>
#include <stddef.h>

struct foo {
    char common[64];

    // This is anonymous.
    union {
        struct {
            char x;
            int y;
            int z;
        };
        struct {
            char u;
            char v;
            char w[0];
        };
    };
};

#define display_offset(member) \
    printf("foo." #member " is at %d\n", \
        offsetof(struct foo, member) \
    )


int main() {
    display_offset(common);
    display_offset(x);
    display_offset(y);
    display_offset(z);
    display_offset(u);
    display_offset(v);
    display_offset(w);
}
