#include "car.h"
#include "object.h"
#include "tools.h"

#include <stdlib.h>


void display_and_destroy(Object* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        int howfar = rand();
        car->drive(car, howfar);
    }

    instance->display(instance);
    instance->destroy(instance);
}

int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123");
    Car* volvo = Car_create("Volvo", "ERF-873");

    display_and_destroy(object);
    display_and_destroy((Object*) saab);
    display_and_destroy((Object*) volvo);

    return 0;
}