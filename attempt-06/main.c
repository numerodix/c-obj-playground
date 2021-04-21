#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int xrandint(int min, int max) {
    return (int) ((double) min + (((double) (max - min) * (double) rand()) / (double) RAND_MAX));
}


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


#define ELECTRIC_CAR_FIELDS \
    int charge_kwhs;

struct _ElectricCar {
    ElectricCarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
    ELECTRIC_CAR_FIELDS
};


typedef void (*Object_Delete)(Object* self);
typedef void (*Object_Display)(Object* self);
typedef int (*Object_Get_Objid)(Object* self);

struct _ObjectVTable {
    void* super;  // never points to anything!
    Object_Delete delete;
    Object_Display display;
    Object_Get_Objid get_objid;
};


typedef void (*Car_Delete)(Car* self);
typedef void (*Car_Display)(Car* self);
typedef int (*Car_Get_Objid)(Car* self);
typedef void (*Car_Drive)(Car* self, int kms);

struct _CarVTable {
    ObjectVTable* super;
    Car_Delete delete;
    Car_Display display;
    Car_Get_Objid get_objid;
    Car_Drive drive;
};


typedef void (*ElectricCar_Delete)(ElectricCar* self);
typedef void (*ElectricCar_Display)(ElectricCar* self);
typedef int (*ElectricCar_Get_Objid)(ElectricCar* self);
typedef void (*ElectricCar_Drive)(ElectricCar* self, int kms);
typedef void (*ElectricCar_Charge)(ElectricCar* self, int kwhs);

struct _ElectricCarVTable {
    CarVTable* super;
    ElectricCar_Delete delete;
    ElectricCar_Display display;
    ElectricCar_Get_Objid get_objid;
    ElectricCar_Drive drive;
    ElectricCar_Charge charge;
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
    .super = NULL,
    .delete = Object_delete,
    .display = Object_display,
    .get_objid = Object_get_objid,
};

Object* Object_create() {
    Object* object = (Object*) calloc(1, sizeof(Object));
    object->vtable = &Object_vtable;
    object->objid = xrandint(1, 1000);
    return object;
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


void Car_delete(Car* self) {
    free((char*) self->make);
    free((char*) self->reg_no);
    free(self);
}

void Car_display(Car* self) {
    printf("Car* at: %p, objid: %d -- make: %s, reg_no: %s, driven: %d kms\n",
           (void*) self, self->vtable->get_objid(self),
           self->make, self->reg_no, self->driven_kms);
}

void Car_drive(Car* self, int kms) {
    self->driven_kms += kms;
}

CarVTable Car_vtable = {
    .super = &Object_vtable,
    .delete = Car_delete,
    .display = Car_display,
    .get_objid = (Car_Get_Objid) Object_get_objid,
    .drive = Car_drive,
};

Car* Car_create(const char* make, const char* reg_no, int driven_kms) {
    Car* car = (Car*) calloc(1, sizeof(Car));
    car->vtable = &Car_vtable;
    car->objid = xrandint(1, 1000);
    car->make = strdup(make);
    car->reg_no = strdup(reg_no);
    car->driven_kms = driven_kms;
    return car;
}

Car* Car_cast(void* instance) {
    CarVTable* vtable = ((Car*) instance)->vtable;

    while (vtable) {
        if (vtable == &Car_vtable) {
            return (Car*) instance;
        }
        vtable = (CarVTable*) vtable->super;
    }

    return NULL;
}


void ElectricCar_charge(ElectricCar* self, int kwhs) {
    self->charge_kwhs += kwhs;
}

void ElectricCar_display(ElectricCar* self) {
    printf("ElectricCar* at: %p, objid: %d -- make: %s, reg_no: %s, driven: %d kms, charge: %d kwhs\n",
           (void*) self, self->vtable->get_objid(self),
           self->make, self->reg_no, self->driven_kms,
           self->charge_kwhs);
}

ElectricCarVTable ElectricCar_vtable = {
    .super = &Car_vtable,
    .delete = (ElectricCar_Delete) Car_delete,
    .display = ElectricCar_display,
    .get_objid = (ElectricCar_Get_Objid) Object_get_objid,
    .drive = (ElectricCar_Drive) Car_drive,
    .charge = ElectricCar_charge,
};

ElectricCar* ElectricCar_create(const char* make, const char* reg_no, int driven_kms,
                                int charge_kwhs) {
    ElectricCar* ecar = (ElectricCar*) calloc(1, sizeof(ElectricCar));
    ecar->vtable = &ElectricCar_vtable;
    ecar->objid = xrandint(1, 1000);
    ecar->make = strdup(make);
    ecar->reg_no = strdup(reg_no);
    ecar->driven_kms = driven_kms;
    ecar->charge_kwhs = charge_kwhs;
    return ecar;
}

ElectricCar* ElectricCar_cast(void* instance) {
    ElectricCarVTable* vtable = ((ElectricCar*) instance)->vtable;

    while (vtable) {
        if (vtable == &ElectricCar_vtable) {
            return (ElectricCar*) instance;
        }
        vtable = (ElectricCarVTable*) vtable->super;
    }

    return NULL;
}



void display_and_delete(void* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        car->vtable->drive(car, xrandint(1, 10));
    }

    ElectricCar* ecar = ElectricCar_cast(instance);
    if (ecar) {
        ecar->vtable->charge(ecar, xrandint(100, 200));
    }

    Object* object = Object_cast(instance);
    if (object) {
        object->vtable->display(object);
        object->vtable->delete(object);
    }
}

int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123", 10);
    Car* volvo = Car_create("Volvo", "SEK-977", 10);
    ElectricCar* tesla = ElectricCar_create("Tesla", "EVP-400", 10, 50);

    display_and_delete(object);
    display_and_delete(saab);
    display_and_delete(volvo);
    display_and_delete(tesla);

    return 0;
}