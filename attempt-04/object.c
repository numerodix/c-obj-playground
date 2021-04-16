#include "object.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const ObjectVTable Object_vtable = {
    .extends = NULL,
    .destroy = Object_destroy,
    .display = Object_display,
};


void Object_destroy(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    char* id = format_object_identifier(instance);
    printf("Object_display for Object* instance %s\n", id);
    free(id);
}

Object* Object_create() {
    Object stack_instance = {
        .vtable = &Object_vtable,
        .objid = rand(),
    };

    Object* instance = (Object*) xcalloc(1, sizeof(Object));
    memcpy(instance, &stack_instance, sizeof(Object));

    return instance;
}

bool Object_is_A(Object* instance, const ObjectVTable* vtable) {
    for (const ObjectVTable* vt = instance->vtable; vt; vt = vt->extends) {
        if (vt == vtable) {
            return true;
        }
    }

    return false;
}

char* format_object_identifier(Object* instance) {
    const int maxlen = 70;  // guessing at the max len here
    char* buf = xcalloc(1, maxlen);
    snprintf(buf, maxlen, "[p: %p, vtable: %p, objid: %lu]",
        (void*) instance, (void*) instance->vtable, instance->objid);
    return buf;
}