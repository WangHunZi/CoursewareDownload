#include <thread.h>
#include "lockdep.h"

lock_t lk1 = LOCK_INIT();
lock_t lk2 = LOCK_INIT();
lock_t lk3 = LOCK_INIT();

struct some_object {
    lock_t lock;
    int data;
};

void object_init(struct some_object *obj) {
    obj->lock = LOCK_INIT();
    obj->data = 100;
}

void create_object() {
    struct some_object *obj = malloc(sizeof(struct some_object));
    assert(obj);
    object_init(obj);

    lock(&obj->lock);
    unlock(&obj->lock);

    free(obj);
}

void T_1() {
    lock(&lk1);
    lock(&lk2);
    create_object();
    unlock(&lk1);
    unlock(&lk2);
}

void T_2() {
    sleep(3);

    lock(&lk2);
    lock(&lk3);
    create_object();
    unlock(&lk2);
    unlock(&lk3);
}

void T_3() {
    sleep(6);

    lock(&lk3);
    lock(&lk1);
    create_object();
    unlock(&lk1);
    unlock(&lk3);
}

int main() {
    create(T_1);
    create(T_2);
    create(T_3);
}
