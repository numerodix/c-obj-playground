#include "car.h"
#include "object.h"
#include "tools.h"

#include <stdlib.h>


void display_and_destroy(Object* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        int howfar = rand();
        car->vtable->drive(car, howfar);
    }

    instance->vtable->subclass_print(instance);
    instance->vtable->display(instance);
    instance->vtable->destroy(instance);
}

int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123");

    display_and_destroy(object);
    display_and_destroy((Object*) saab);

    return 0;
}