#ifndef CAR_H
#define CAR_H

#include "object.h"

#include <stdint.h>


typedef struct Car_ Car;

struct Car_ {
    OBJECT_MEMBERS

    const char* make;
    const char* reg_no;
};

Car* Car_create(const char* make, const char* reg_no);

#endif
