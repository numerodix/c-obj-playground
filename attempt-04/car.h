#ifndef CAR_H
#define CAR_H

#include "object.h"
#include "typeid.h"

#include <stdint.h>


typedef struct Car_ Car;
typedef struct CarVTable_ CarVTable;

// these are the types of the additional methods that Car has
typedef void(*Car_Drive)(Car* instance, int num_kms);

struct CarVTable_ {
    const ObjectVTable super;
    const Car_Drive drive;
};

struct Car_ {
    const Object super;
    const char* const make;
    const char* const reg_no;
    int driven;
};

const CarVTable Car_vtable;

Car* Car_create(const char* make, const char* reg_no);
void Car_destroy(Car *instance);
void Car_display(Car *instance);
void Car_drive(Car* instance, int num_kms);

Car* Car_cast(Object* instance);

#endif
