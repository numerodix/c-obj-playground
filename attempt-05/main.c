#include "car.h"
#include "ecar.h"


void display_and_delete(gpointer obj) {
    AppCar* car = APP_CAR(obj);
    app_car_drive(car, 3);

    if (APP_IS_ELECTRIC_CAR(car)) {
        AppElectricCar* ecar = APP_ELECTRIC_CAR(car);
        app_electric_car_charge(ecar, 7);
    }

    app_car_display(car);
    g_clear_object(&obj);
}

int main() {
    AppCar* saab = app_car_new("Saab", "ABC-123");
    AppElectricCar* tesla = app_electric_car_new("Tesla", "REY-570", 50);

    // ((AppCarClass*) saab->parent.g_type_instance.g_class)->drive(saab, 2);

    // ((AppCarClass*) saab->parent.g_type_instance.g_class)->display(saab);
    // ((AppElectricCarClass*) tesla->parent.parent.g_type_instance.g_class)->charge(tesla, -1);

    display_and_delete(saab);
    display_and_delete(tesla);

    return 0;
}