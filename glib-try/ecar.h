#ifndef ECAR_H
#define ECAR_H

#include "car.h"

#include <glib-object.h>


typedef struct _AppElectricCar AppElectricCar;
typedef struct _AppElectricCarClass AppElectricCarClass;

typedef void (*AppElectricCar_Charge)(AppElectricCar* self, gint kwhs);

struct _AppElectricCar {
    AppCar parent;
    gint charge_kwhs;
};

struct _AppElectricCarClass {
    AppCarClass parent;
    AppElectricCar_Charge charge;
};

GType app_electric_car_get_type (void);

#define APP_TYPE_ELECTRIC_CAR app_electric_car_get_type ()
#define APP_ELECTRIC_CAR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
                               APP_TYPE_ELECTRIC_CAR, AppElectricCar))
#define APP_ELECTRIC_CAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), \
                                         APP_TYPE_ELECTRIC_CAR, AppElectricCarClass))
#define APP_IS_ELECTRIC_CAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE(obj, \
                                                             app_electric_car_get_type()))

AppElectricCar* app_electric_car_new(const gchar* make, const gchar* reg_no,
                                     gint charge_kwhs);
void app_electric_car_charge(AppElectricCar* self, gint kwhs);

void app_electric_car_charge_impl(AppElectricCar* self, gint kwhs);
void app_electric_car_display_impl(AppElectricCar* self);

#endif