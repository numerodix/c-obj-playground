#include "object.h"
#include "tools.h"
#include "typeid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void Object_destroy(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    char* id = format_object_identifier(instance);
    printf("Object_display for Object* instance %s\n", id);
    free(id);
}

Object* Object_create() {
    // NOTE: because the func pointer fields are const we cannot malloc an
    // Object* and then assign to these fields. Instead we have to declare
    // Object on the stack and populate the fields at declaration time. Then, we
    // allocate an Object* and copy the contents of our stack Object into the
    // heap Object*. This works because the int/ptr fields are memcpy'able, but
    // it's not pretty.
    Object stack_instance = {
        .destroy = Object_destroy,
        .display = Object_display,
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