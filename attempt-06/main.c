#include <stdio.h>
#include <stdlib.h>


typedef struct _Object Object;
typedef struct _Car Car;
typedef struct _ElectricCar ElectricCar;

typedef struct _ObjectVTable ObjectVTable;
typedef struct _CarVTable CarVTable;
typedef struct _ElectricCarVTable ElectricCarVTable;


#define OBJECT_FIELDS \
    int objid;

struct _Object {
    ObjectVTable* vtable;
    OBJECT_FIELDS
};


#define CAR_FIELDS \
    const char* make; \
    const char* reg_no; \
    int driven_kms;

struct _Car {
    CarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
};


typedef void (*Object_Delete)(Object* self);
typedef void (*Object_Display)(Object* self);
typedef int (*Object_Get_Objid)(Object* self);

struct _ObjectVTable {
    Object_Delete delete;
    Object_Display display;
    Object_Get_Objid get_objid;
};


typedef void (*Car_Delete)(Car* self);
typedef void (*Car_Display)(Car* self);
typedef int (*Car_Get_Objid)(Car* self);
typedef void (*Car_Drive)(Car* self, int kms);

struct _CarVTable {
    Car_Delete delete;
    Car_Display display;
    Car_Get_Objid get_objid;
    Car_Drive drive;
};


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

ObjectVTable Object_vtable = {
    .delete = Object_delete,
    .display = Object_display,
    .get_objid = Object_get_objid,
};


void Car_display(Car* self) {
    printf("Car* at: %p, objid: %d -- make: %s, reg_no: %s, driven: %d kms\n",
           (void*) self, self->vtable->get_objid(self),
           self->make, self->reg_no, self->driven_kms);
}

void Car_drive(Car* self, int kms) {
    self->driven_kms += kms;
}

CarVTable Car_vtable = {
    .delete = (Car_Delete) Object_delete,
    .display = Car_display,
    .drive = Car_drive,
    .get_objid = (Car_Get_Objid) Object_get_objid,
};


void display_and_delete(void* instance) {
    if (((Car*) instance)->vtable == &Car_vtable) {
        Car* car = (Car*) instance;
        car->vtable->drive(car, 8);
    }

    Object* object = (Object*) instance;
    object->vtable->display(object);
    object->vtable->delete(object);
}

int main() {
    Object* object = (Object*) calloc(1, sizeof(Object));
    object->vtable = &Object_vtable;
    object->objid = rand();

    Car* car = (Car*) calloc(1, sizeof(Car));
    car->vtable = &Car_vtable;
    car->objid = rand();
    car->make = "Saab";
    car->reg_no = "ABC-123";
    car->driven_kms = 13;

    display_and_delete(object);
    display_and_delete(car);

    return 0;
}