#include "object.h"
#include "tools.h"
#include "typeid.h"

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
        .typeid = TYPEID_OBJECT,
        .objid = rand(),
    };

    Object* instance = (Object*) xcalloc(1, sizeof(Object));
    memcpy(instance, &stack_instance, sizeof(Object));

    return instance;
}

char* format_object_identifier(Object* instance) {
    const int maxlen = 60;  // guessing at the max len here
    char* buf = xcalloc(1, maxlen);
    snprintf(buf, maxlen, "[p: %p, typeid: %lu, objid: %lu]",
        (void*) instance, instance->typeid, instance->objid);
    return buf;
}