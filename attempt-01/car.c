#include "car.h"
#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void Car_destroy(Car *instance) {
    free((void*) instance->make);
    free((void*) instance->reg_no);

    free(instance);
}

void Car_display(Car *instance) {
    printf("Car_display for Car* instance [%p] objid: %lu make: %s reg_no: %s\n", 
        (void*) instance, instance->objid, instance->make, instance->reg_no);
}

Car* Car_create(const char* make, const char* reg_no) {
    Car stack_instance = {
        .destroy = Car_destroy,
        .display = Car_display,
        .objid = rand(),
        .make = xstrdup(make),
        .reg_no = xstrdup(reg_no),
    };

    Car* instance = (Car*) xcalloc(1, sizeof(Car));
    memcpy(instance, &stack_instance, sizeof(Car));

    return instance;
}
