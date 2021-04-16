#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stdint.h>


typedef struct Object_ Object;
typedef struct ObjectVTable_ ObjectVTable;

// these are the types of the methods that Object has
typedef void(*Object_Destroy)(Object* instance);
typedef void(*Object_Display)(Object* instance);

struct ObjectVTable_ {
    const ObjectVTable* extends;
    const Object_Destroy destroy;
    const Object_Display display;
};

struct Object_ {
    const ObjectVTable* vtable;
    const uint64_t objid;
};

const ObjectVTable Object_vtable;

Object* Object_create();
void Object_destroy(Object *instance);
void Object_display(Object *instance);

bool Object_is_A(Object* instance, const ObjectVTable* vtable);

// Returns a malloc'd string: [p: 0x55cc93b4d2a0, vtable: 0x55d3711fdd20, objid: 1804289383]
char* format_object_identifier(Object* instance);

#endif