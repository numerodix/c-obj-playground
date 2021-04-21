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

#define OBJECT_VTABLE_FIELDS \
    Object_Delete delete; \
    Object_Display display;

struct _ObjectVTable {
    OBJECT_VTABLE_FIELDS
};


typedef void (*Car_Drive)(Car* self, int kms);

#define CAR_VTABLE_FIELDS \
    Car_Drive drive;

struct _CarVTable {
    OBJECT_VTABLE_FIELDS
    CAR_VTABLE_FIELDS
};


void Object_delete(Object* self) {
    free(self);
}

void Object_display(Object* self) {
    printf("Object* at: %p, objid: %d\n", (void*) self, self->objid);
}

ObjectVTable Object_vtable = {
    .delete = Object_delete,
    .display = Object_display,
};


void Car_display(Car* self) {
    printf("Car* at: %p, objid: %d -- make: %s, reg_no: %s, driven: %d kms\n",
           (void*) self, self->objid, self->make, self->reg_no, self->driven_kms);
}

void Car_drive(Car* self, int kms) {
    self->driven_kms += kms;
}

CarVTable Car_vtable = {
    .delete = Object_delete,
    .display = (Object_Display) Car_display,
    .drive = Car_drive,
};


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

    car->vtable->drive(car, 8);

    object->vtable->display(object);
    car->vtable->display(car);

    object->vtable->delete(object);
    car->vtable->delete(car);

    return 0;
}