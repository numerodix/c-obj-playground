#include "car.h"
#include "ecar.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const ElectricCarVTable ElectricCar_vtable = {
    .super = {
        .super = {
            .extends = (const ObjectVTable*) &Car_vtable,
            .destroy = (Object_Destroy) Car_destroy,
            .display = (Object_Display) ElectricCar_display,
        },
        .drive = Car_drive,
    },
    .charge = ElectricCar_charge,
};


void ElectricCar_display(ElectricCar *instance) {
    char* id = format_object_identifier((Object*) instance);
    printf("ElectricCar_display for ElectricCar* instance %s, make: %s, reg_no: %s, driven: %d kms, capacity: %d kwhs\n",
        id, instance->super.make, instance->super.reg_no, instance->super.driven,
        instance->charge_kwhs);
    free(id);
}

void ElectricCar_charge(ElectricCar* instance, int kwhs) {
    instance->charge_kwhs += kwhs;
}

ElectricCar* ElectricCar_create(const char* make, const char* reg_no, 
                                int charge_kwhs) {
    ElectricCar stack_instance = {
        .super = {
            .super = {
                .vtable = (const ObjectVTable*) &ElectricCar_vtable,
                .objid = rand(),
            },
            .make = xstrdup(make),
            .reg_no = xstrdup(reg_no),
            .driven = 0,
        },
        .charge_kwhs = charge_kwhs,
    };

    ElectricCar* instance = (ElectricCar*) xcalloc(1, sizeof(ElectricCar));
    memcpy(instance, &stack_instance, sizeof(ElectricCar));

    return instance;
}