#include "car.h"
#include "ecar.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void ElectricCar_destroy(ElectricCar *instance) {
    Car* car = Car_cast((Object*) instance);

    free((void*) car->make);
    free((void*) car->reg_no);

    free(car);
}

void ElectricCar_display(ElectricCar *instance) {
    char* id = format_object_identifier((Object*) instance);
    printf("ElectricCar_display for ElectricCar* instance %s, make: %s, reg_no: %s, driven: %d kms, capacity: %d kwhs\n",
        id, instance->super.make, instance->super.reg_no, instance->super.driven,
        instance->charge_kwhs);
    free(id);
}

void ElectricCar_drive(ElectricCar* instance, int num_kms) {
    Car* car = Car_cast((Object*) instance);
    car->driven += num_kms;
}

void ElectricCar_charge(ElectricCar* instance, int kwhs) {
    instance->charge_kwhs += kwhs;
}

ElectricCar* ElectricCar_create(const char* make, const char* reg_no, 
                                int charge_kwhs) {
    ElectricCar stack_instance = {
        .super = {
            .super = {
                .destroy = (Object_Destroy) ElectricCar_destroy,
                .display = (Object_Display) ElectricCar_display,
                .typeid = TYPEID_ELECTRIC_CAR,
                .objid = rand(),
            },

            .drive = (Car_Drive) ElectricCar_drive,
            .make = xstrdup(make),
            .reg_no = xstrdup(reg_no),
            .driven = 0,
        },

        .charge = ElectricCar_charge,
        .charge_kwhs = charge_kwhs,
    };

    ElectricCar* instance = (ElectricCar*) xcalloc(1, sizeof(ElectricCar));
    memcpy(instance, &stack_instance, sizeof(ElectricCar));

    return instance;
}

ElectricCar* ElectricCar_cast(Object* instance) {
    if (instance->typeid != TYPEID_ELECTRIC_CAR) {
        return NULL;
    }

    return (ElectricCar*) instance;
}