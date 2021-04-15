#ifndef CAR_H
#define CAR_H

#include "object.h"
#include "typeid.h"

#include <stdint.h>


typedef struct Car_ Car;
typedef struct CarVTable_ CarVTable;

// these are the types of the additional methods that Car has
typedef void(*Car_Drive)(Car* instance, int num_kms);
typedef void(*Car_Print)(Car* instance);

struct Car_ {
    const Object super;

    const CarVTable* vtable;
    const char* const make;
    const char* const reg_no;
    int driven;
};

struct CarVTable_ {
    const Car_Drive drive;
    const Car_Print print;
};

Car* Car_create(const char* make, const char* reg_no);
void Car_destroy(Car *instance);
void Car_display(Car *instance);
void Car_drive(Car* instance, int num_kms);
void Car_print(Car* instance);

Car* Car_cast(Object* instance);

#endif
