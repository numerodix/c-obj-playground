#include "object.h"
#include "tools.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


ObjectVTable Object_vtable = {
    .super = NULL,
    .delete = Object_delete,
    .display = Object_display,
    .get_objid = Object_get_objid,
};


Object* Object_create() {
    Object* object = (Object*) xcalloc(1, sizeof(Object));
    object->vtable = &Object_vtable;
    object->objid = xrandint(1, 1000);
    return object;
}

void Object_delete(Object* self) {
    free(self);
}

void Object_display(Object* self) {
    printf("Object* at: %p, objid: %d\n",
           (void*) self, self->vtable->get_objid(self));
}

int Object_get_objid(Object* self) {
    return self->objid;
}

Object* Object_cast(void* instance) {
    ObjectVTable* vtable = ((Object*) instance)->vtable;

    while (vtable) {
        if (vtable == &Object_vtable) {
            return (Object*) instance;
        }
        vtable = vtable->super;
    }

    return NULL;
}