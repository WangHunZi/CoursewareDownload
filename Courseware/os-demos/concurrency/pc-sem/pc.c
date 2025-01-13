#include <thread.h>
#include <thread-sync.h>

sem_t fill, empty;

void T_produce() {
    while (1) {
        // Needs an empty slot for producing.
        P(&empty);

        printf("(");

        // Creates a filled slot.
        V(&fill);
    }
}

void T_consume() {
    while (1) {
        // Needs a filled slot for consuming.
        P(&fill);

        printf(")");
        
        // Creates an empty slot.
        V(&empty);
    }
}

int main(int argc, char *argv[]) {
    assert(argc == 2);

    // Initially, 0 filled, n empty
    SEM_INIT(&fill, 0);
    SEM_INIT(&empty, atoi(argv[1]));

    for (int i = 0; i < 8; i++) {
        create(T_produce);
        create(T_consume);
    }
}
