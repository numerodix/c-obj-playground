#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

typedef struct Object_ Object;

typedef void(*Object_Destroy)(Object* instance);
typedef void(*Object_Display)(Object* instance);

struct Object_ {
    Object_Destroy destroy;
    Object_Display display;
    uint64_t objid;
};

Object* Object_create();

#endif