#ifndef ECAR_H
#define ECAR_H

#include "car.h"


typedef struct _ElectricCar ElectricCar;
typedef struct _ElectricCarVTable ElectricCarVTable;

typedef void (*ElectricCar_Delete)(ElectricCar* self);
typedef void (*ElectricCar_Display)(ElectricCar* self);
typedef int (*ElectricCar_Get_Objid)(ElectricCar* self);
typedef void (*ElectricCar_Drive)(ElectricCar* self, int kms);
typedef void (*ElectricCar_Charge)(ElectricCar* self, int kwhs);

struct _ElectricCarVTable {
    CarVTable* super;
    ElectricCar_Delete delete;
    ElectricCar_Display display;
    ElectricCar_Get_Objid get_objid;
    ElectricCar_Drive drive;
    ElectricCar_Charge charge;
};


#define ELECTRIC_CAR_FIELDS \
    int charge_kwhs;

struct _ElectricCar {
    ElectricCarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
    ELECTRIC_CAR_FIELDS
};

ElectricCar* ElectricCar_create(const char* make, const char* reg_no, int driven_kms,
                                int charge_kwhs);
void ElectricCar_charge(ElectricCar* self, int kwhs);
void ElectricCar_display(ElectricCar* self);
ElectricCar* ElectricCar_cast(void* instance);

#endif