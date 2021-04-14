#include "object.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>


void Object_destroy(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    printf("Object_display for Object* instance objid: %lu at: %p\n", 
        instance->objid, (void*) instance);
}

Object* Object_create() {
    Object* instance = (Object*) xcalloc(1, sizeof(Object));

    instance->objid = rand();
    instance->destroy = Object_destroy;
    instance->display = Object_display;

    return instance;
}
