#include "car.h"
#include "ecar.h"

#include <glib.h>


void display(gpointer obj) {
    AppCar* car = APP_CAR(obj);
    app_car_drive(car, 3);

    if (APP_IS_ELECTRIC_CAR(car)) {
        AppElectricCar* ecar = APP_ELECTRIC_CAR(car);
        app_electric_car_charge(ecar, 7);
    }

    app_car_display(car);
}

int main() {
    AppCar* saab = app_car_new("Saab", "ABC-123");
    AppElectricCar* tesla = app_electric_car_new("Tesla", "REY-570", 50);

    display(G_OBJECT(saab));
    display(G_OBJECT(tesla));

    return 0;
}