#ifndef CAR_H
#define CAR_H

#include "object.h"
#include "typeid.h"

#include <stdint.h>


typedef struct Car_ Car;

// these are the types of the additional methods that Car has
typedef void(*Car_Drive)(Car* instance, int num_kms);

struct Car_ {
    const Object super;

    const Car_Drive drive;
    const char* const make;
    const char* const reg_no;
    int driven;
};

Car* Car_create(const char* make, const char* reg_no);

Car* Car_cast(Object* instance);

void Car_destroy(Car *instance);
void Car_drive(Car* instance, int num_kms);

#endif
