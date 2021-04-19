#include "car.h"

#include <glib.h>
#include <glib/gprintf.h>


int main() {
    AppCar* car = app_car_new("Saab", "ABC-123");

    app_car_drive(car, 2);

    app_car_display(car);

    return 0;
}