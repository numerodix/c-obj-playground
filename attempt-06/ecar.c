#include "ecar.h"
#include "object.h"
#include "tools.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


ElectricCarVTable ElectricCar_vtable = {
    .super = &Car_vtable,
    .delete = (ElectricCar_Delete) Car_delete,
    .display = ElectricCar_display,
    .get_objid = (ElectricCar_Get_Objid) Object_get_objid,
    .drive = (ElectricCar_Drive) Car_drive,
    .charge = ElectricCar_charge,
};


ElectricCar* ElectricCar_create(const char* make, const char* reg_no, int driven_kms,
                                int charge_kwhs) {
    ElectricCar* ecar = (ElectricCar*) xcalloc(1, sizeof(ElectricCar));
    Object_init((Object*) ecar);
    Car_init((Car*) ecar, make, reg_no, driven_kms);
    ElectricCar_init(ecar, charge_kwhs);
    ecar->vtable = &ElectricCar_vtable;
    return ecar;
}

void ElectricCar_init(ElectricCar* self, int charge_kwhs) {
    self->charge_kwhs = charge_kwhs;
}

void ElectricCar_charge(ElectricCar* self, int kwhs) {
    self->charge_kwhs += kwhs;
}

void ElectricCar_display(ElectricCar* self) {
    printf("ElectricCar* at: %p, objid: %d -- "
           "make: %s, reg_no: %s, driven: %d kms, charge: %d kwhs\n",
           (void*) self, self->vtable->get_objid(self),
           self->make, self->reg_no, self->driven_kms,
           self->charge_kwhs);
}

ElectricCar* ElectricCar_cast(void* instance) {
    ElectricCarVTable* vtable = ((ElectricCar*) instance)->vtable;

    while (vtable) {
        if (vtable == &ElectricCar_vtable) {
            return (ElectricCar*) instance;
        }
        vtable = (ElectricCarVTable*) vtable->super;
    }

    return NULL;
}
