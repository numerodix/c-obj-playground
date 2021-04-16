#include "car.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const CarVTable Car_vtable = {
    .super = {
        .extends = &Object_vtable,
        .destroy = (Object_Destroy) Car_destroy,
        .display = (Object_Display) Car_display,
    },
    .drive = Car_drive,
};


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

Car* Car_create(const char* make, const char* reg_no) {
    Car stack_instance = {
        .super = {
            .vtable = (const ObjectVTable*) &Car_vtable,
            .objid = rand(),
        },
        .make = xstrdup(make),
        .reg_no = xstrdup(reg_no),
        .driven = 0,
    };

    Car* instance = (Car*) xcalloc(1, sizeof(Car));
    memcpy(instance, &stack_instance, sizeof(Car));

    return instance;
}