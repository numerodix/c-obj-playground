#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>


typedef struct Object_ Object;

// these are the types of the methods that Object has
typedef void(*Object_Destroy)(Object* instance);
typedef void(*Object_Display)(Object* instance);

struct Object_ {
    // methods are const pointers so that the user cannot re-assign them
    const Object_Destroy destroy;
    const Object_Display display;
    // unique id for every Object
    uint64_t objid;
};

// Object_create needs to be declared because users will need it to obtain an
// Object*. Object_destroy and Object_display are not, so that they are only
// accessible through an Object* instance.
Object* Object_create();

#endif