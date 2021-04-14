#ifndef CAR_H
#define CAR_H

#include <stdint.h>


typedef struct Car_ Car;

typedef void(*Car_Destroy)(Car* instance);
typedef void(*Car_Display)(Car* instance);

struct Car_ {
    const Car_Destroy destroy;
    const Car_Display display;
    uint64_t objid;
    const char* make;
    const char* reg_no;
};

Car* Car_create(const char* make, const char* reg_no);

#endif
