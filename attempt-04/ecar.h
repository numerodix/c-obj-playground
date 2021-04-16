#ifndef ECAR_H
#define ECAR_H

#include "car.h"
#include "typeid.h"

#include <stdint.h>


typedef struct ElectricCar_ ElectricCar;
typedef struct ElectricCarVTable_ ElectricCarVTable;

// these are the types of the additional methods that ElectricCar has
typedef void(*ElectricCar_Charge)(ElectricCar* instance, int kwhs);

struct ElectricCarVTable_ {
    const CarVTable super;
    const ElectricCar_Charge charge;
};

struct ElectricCar_ {
    const Car super;
    int charge_kwhs;
};

const ElectricCarVTable ElectricCar_vtable;

ElectricCar* ElectricCar_create(const char* make, const char* reg_no,
                                int charge_kwhs);
void ElectricCar_charge(ElectricCar* instance, int kwhs);
void ElectricCar_display(ElectricCar *instance);

ElectricCar* ElectricCar_cast(Object* instance);

#endif
