#include "car.h"
#include "ecar.h"
#include "object.h"
#include "tools.h"

#include <stdlib.h>


void display_and_destroy(Object* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        int howfar = rand();
        car->drive(car, howfar);
    }

    ElectricCar* ecar = ElectricCar_cast(instance);
    if (ecar) {
        ecar->charge(ecar, 7);
    }

    instance->display(instance);
    instance->destroy(instance);
}

int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123");
    ElectricCar* tesla = ElectricCar_create("Tesla", "REY-570", 50);

    display_and_destroy(object);
    display_and_destroy((Object*) saab);
    display_and_destroy((Object*) tesla);

    return 0;
}