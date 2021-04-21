#ifndef CAR_H
#define CAR_H

#include "object.h"


typedef struct _Car Car;
typedef struct _CarVTable CarVTable;

typedef void (*Car_Delete)(Car* self);
typedef void (*Car_Display)(Car* self);
typedef int (*Car_Get_Objid)(Car* self);
typedef void (*Car_Drive)(Car* self, int kms);

struct _CarVTable {
    ObjectVTable* super;
    Car_Delete delete;
    Car_Display display;
    Car_Get_Objid get_objid;
    Car_Drive drive;
};

CarVTable Car_vtable;


#define CAR_FIELDS \
    const char* make; \
    const char* reg_no; \
    int driven_kms;

struct _Car {
    CarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
};

Car* Car_create(const char* make, const char* reg_no, int driven_kms);
void Car_init(Car* self, const char* make, const char* reg_no, int driven_kms);
void Car_delete(Car* self);
void Car_display(Car* self);
void Car_drive(Car* self, int kms);
Car* Car_cast(void* instance);

#endif
