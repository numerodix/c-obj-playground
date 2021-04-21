#ifndef OBJECT_H
#define OBJECT_H


typedef struct _Object Object;
typedef struct _ObjectVTable ObjectVTable;

typedef void (*Object_Delete)(Object* self);
typedef void (*Object_Display)(Object* self);
typedef int (*Object_Get_Objid)(Object* self);

struct _ObjectVTable {
    void* super;  // never points to anything!
    Object_Delete delete;
    Object_Display display;
    Object_Get_Objid get_objid;
};

ObjectVTable Object_vtable;


#define OBJECT_FIELDS \
    int objid;

struct _Object {
    ObjectVTable* vtable;
    OBJECT_FIELDS
};

Object* Object_create();
void Object_delete(Object* self);
void Object_display(Object* self);
int Object_get_objid(Object* self);
Object* Object_cast(void* instance);

#endif