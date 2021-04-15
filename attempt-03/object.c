#include "car.h"
#include "object.h"
#include "tools.h"
#include "typeid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W(varname) (varname ## __Object)


const ObjectVTable W(Object_vtable) = {
    .destroy = Object_destroy,
    .display = Object_display,
    .get_objid = Object_getObjId,
    .subclass_print = Object_subclassPrint,
};


Object* Object_create() {
    Object stack_instance = {
        .vtable = &W(Object_vtable),
        .typeid = TYPEID_OBJECT,
        .objid = rand(),
    };

    Object* instance = (Object*) xcalloc(1, sizeof(Object));
    memcpy(instance, &stack_instance, sizeof(Object));

    return instance;
}

void Object_destroy(Object *instance) {
    free(instance);
}

void Object_display(Object *instance) {
    char* id = format_object_identifier(instance);
    printf("Object_display for Object* instance %s\n", id);
    free(id);
}

uint64_t Object_getObjId(Object* instance) {
    return instance->objid;
}

void Object_subclassPrint(Object *instance) {
    printf("Object.subclassPrint calling Car.print: ");
    Car* car = Car_cast(instance);
    if (car) {
        car->vtable->print(car);
    } else {
        printf("cannot - it's not a Car\n");
    }
}

char* format_object_identifier(Object* instance) {
    const int maxlen = 60;  // guessing at the max len here
    char* buf = xcalloc(1, maxlen);
    snprintf(buf, maxlen, "[p: %p, typeid: %lu, objid: %lu]",
        (void*) instance, instance->typeid, instance->objid);
    return buf;
}