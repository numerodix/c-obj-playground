#include "object.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void Object_destroy(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    printf("Object_display for Object* instance objid: %lu at: %p\n", 
        instance->objid, (void*) instance);
}

Object* Object_create() {
    // NOTE: because the func pointer fields are const we cannot malloc an
    // Object* and then assign to these fields. Instead we have to declare
    // Object on the stack and populate the fields at declaration time. Then, we
    // allocate an Object* and copy the contents of our stack Object into the
    // heap Object*. This works because the int/ptr fields are memcpy'able, but
    // it's not pretty.
    Object stack_instance = {
        .objid = rand(),
        .destroy = Object_destroy,
        .display = Object_display,
    };

    Object* instance = (Object*) xcalloc(1, sizeof(Object));
    memcpy(instance, &stack_instance, sizeof(Object));

    return instance;
}
