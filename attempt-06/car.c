#include "car.h"
#include "tools.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


CarVTable Car_vtable = {
    .super = &Object_vtable,
    .delete = Car_delete,
    .display = Car_display,
    .get_objid = (Car_Get_Objid) Object_get_objid,
    .drive = Car_drive,
};


Car* Car_create(const char* make, const char* reg_no, int driven_kms) {
    Car* car = (Car*) xcalloc(1, sizeof(Car));
    Object_init((Object*) car);
    Car_init(car, make, reg_no, driven_kms);
    car->vtable = &Car_vtable;
    return car;
}

void Car_init(Car* self, const char* make, const char* reg_no, int driven_kms) {
    self->make = xstrdup(make);
    self->reg_no = xstrdup(reg_no);
    self->driven_kms = driven_kms;
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
