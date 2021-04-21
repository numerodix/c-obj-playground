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

#define OBJECT_CAST(self) (Object*)(self)


#define CAR_FIELDS \
    const char* make; \
    const char* reg_no; \
    int driven_kms;

struct _Car {
    CarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
};

#define CAR_CAST(self) (Car*)(self)


typedef void (*Object_Delete)(void* self);
typedef void (*Object_Display)(void* self);

#define OBJECT_VTABLE_FIELDS \
    Object_Delete delete; \
    Object_Display display;

struct _ObjectVTable {
    OBJECT_VTABLE_FIELDS
};


typedef void (*Car_Drive)(void* self, int kms);

#define CAR_VTABLE_FIELDS \
    Car_Drive drive;

struct _CarVTable {
    OBJECT_VTABLE_FIELDS
    CAR_VTABLE_FIELDS
};


void Object_delete(void* self) {
    Object* object = OBJECT_CAST(self);
    free(object);
}

void Object_display(void* self) {
    Object* object = OBJECT_CAST(self);
    printf("Object* at: %p, objid: %d\n", (void*) object, object->objid);
}

ObjectVTable Object_vtable = {
    .delete = Object_delete,
    .display = Object_display,
};


void Car_display(void* self) {
    Car* car = CAR_CAST(self);
    printf("Car* at: %p, objid: %d -- make: %s, reg_no: %s, driven: %d kms\n",
           (void*) self, car->objid, car->make, car->reg_no, car->driven_kms);
}

void Car_drive(void* self, int kms) {
    Car* car = CAR_CAST(self);
    car->driven_kms += kms;
}

CarVTable Car_vtable = {
    .delete = Object_delete,
    .display = Car_display,
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