#include "tools.h"

#include <stdio.h>


typedef struct Object_ Object;

typedef void(*Object_Delete)(Object* instance);
typedef void(*Object_Display)(Object* instance);

struct Object_ {
    Object_Delete delete;
    Object_Display display;
};

void Object_delete(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    printf("Object_display for Object* instance at: %p\n", (void*) instance);
}

Object* Object_create() {
    Object* instance = (Object*) xcalloc(1, sizeof(Object));

    instance->delete = Object_delete;
    instance->display = Object_display;

    return instance;
}


int main() {
    Object* instance = Object_create();

    instance->display(instance);
    instance->delete(instance);

    return 0;
}