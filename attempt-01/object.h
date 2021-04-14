#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>


typedef struct Object_ Object;

// these are the types of the methods that Object has
typedef void(*Object_Destroy)(Object* instance);
typedef void(*Object_Display)(Object* instance);

// there are the fields that every class struct must contain at the beginning of
// the struct - so that it can be cast to Object* and calling the methods will work
#define OBJECT_MEMBERS \
    const Object_Destroy destroy; \
    const Object_Display display; \
    uint64_t objid;

struct Object_ {
    OBJECT_MEMBERS
};

// Object_create needs to be declared because users will need it to obtain an
// Object*. Object_destroy and Object_display are not, so that they are only
// accessible through an Object* instance.
Object* Object_create();

#endif