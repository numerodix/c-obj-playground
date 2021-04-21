#include "car.h"
#include "ecar.h"
#include "object.h"
#include "tools.h"


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
