#include "ecar.h"

#include <glib/gprintf.h>


static void AppElectricCarClassInit(AppElectricCarClass* class, gpointer class_data) {
    (void) class_data;
    class->parent.display = (AppCar_Display) app_electric_car_display_impl;
    class->parent.drive = app_car_drive_impl;
    class->charge = app_electric_car_charge_impl;
}

GType app_electric_car_get_type (void)
{
  static GType type = 0;
  if (type == 0) {
      const GTypeInfo info = {
          sizeof(AppElectricCarClass),
          NULL, /* base_init */
          NULL, /* base_finalize */
          (GClassInitFunc) AppElectricCarClassInit,  /* class_init */
          NULL, /* class_finalize */
          NULL, /* class_data */
          sizeof(AppElectricCar),
          0,    /* n_preallocs */
          NULL, /* instance_init */
          NULL, /* value_table */
      };
      type = g_type_register_static(G_TYPE_OBJECT, "AppElectricCar", &info, 0);
  }
  return type;
}


AppElectricCar* app_electric_car_new(const gchar* make, const gchar* reg_no,
                                     gint charge_kwhs) {
    AppElectricCar* ecar = APP_ELECTRIC_CAR(g_object_new(APP_TYPE_ELECTRIC_CAR, NULL));
    ecar->charge_kwhs = charge_kwhs;

    AppCar* car = APP_CAR(ecar);
    car->make = (gchar*) make;
    car->reg_no = (gchar*) reg_no;
    car->driven_kms = 0;

    return ecar;
}

void app_electric_car_charge(AppElectricCar* self, gint kwhs) {
    AppElectricCarClass *klass;

    klass = APP_ELECTRIC_CAR_GET_CLASS(self);
    g_return_if_fail(klass->charge != NULL);

    klass->charge(self, kwhs);
}

void app_electric_car_charge_impl(AppElectricCar* self, gint kwhs) {
    self->charge_kwhs += kwhs;
}

void app_electric_car_display_impl(AppElectricCar* self) {
    AppCar* car = APP_CAR(self);
    g_printf("AppElectricCar*: make: %s, reg_no %s, driven: %d kms, charge: %d kwhw\n",
             (char*) car->make,
             (char*) car->reg_no,
             (int) car->driven_kms,
             (int) self->charge_kwhs);
}