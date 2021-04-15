#include "car.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W(varname) (varname ## __Car)


const ObjectVTable W(Object_vtable) = {
    .destroy = (Object_Destroy) Car_destroy,
    .display = (Object_Display) Car_display,
    .get_objid = Object_getObjId,
    .subclass_print = Object_subclassPrint,
};

const CarVTable W(Car_vtable) = {
    .drive = Car_drive,
    .print = Car_print,
};


Car* Car_create(const char* make, const char* reg_no) {
    Car stack_instance = {
        .super = {
            .vtable = &W(Object_vtable),
            .typeid = TYPEID_CAR,
            .objid = rand(),
        },

        .vtable = &W(Car_vtable),
        .make = xstrdup(make),
        .reg_no = xstrdup(reg_no),
        .driven = 0,
    };

    Car* instance = (Car*) xcalloc(1, sizeof(Car));
    memcpy(instance, &stack_instance, sizeof(Car));

    return instance;
}

void Car_destroy(Car *instance) {
    free((void*) instance->make);
    free((void*) instance->reg_no);

    free(instance);
}

void Car_display(Car *instance) {
    char* id = format_object_identifier((Object*) instance);
    printf("Car_display for Car* instance %s, make: %s, reg_no: %s, driven: %d kms\n",
        id, instance->make, instance->reg_no, instance->driven);
    free(id);
}

void Car_drive(Car* instance, int num_kms) {
    instance->driven += num_kms;
}

void Car_print(Car* instance) {
    uint64_t objid = instance->super.vtable->get_objid((Object*) instance);
    printf("Printing car with objid: %lu\n", objid);
}

Car* Car_cast(Object* instance) {
    if (instance->typeid != TYPEID_CAR) {
        return NULL;
    }

    return (Car*) instance;
}