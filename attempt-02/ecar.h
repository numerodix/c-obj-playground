#ifndef ECAR_H
#define ECAR_H

#include "car.h"
#include "typeid.h"

#include <stdint.h>


typedef struct ElectricCar_ ElectricCar;

// these are the types of the additional methods that Car has
typedef void(*ElectricCar_Charge)(ElectricCar* instance, int kwhs);

struct ElectricCar_ {
    const Car super;

    const ElectricCar_Charge charge;
    int charge_kwhs;
};

ElectricCar* ElectricCar_create(const char* make, const char* reg_no,
                                int charge_kwhs);

ElectricCar* ElectricCar_cast(Object* instance);

#endif
