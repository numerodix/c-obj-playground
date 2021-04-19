#include "car.h"

#include <glib/gprintf.h>


static void AppCarClassInit(AppCarClass* class, gpointer class_data) {
    (void) class_data;
    class->display = app_car_display_impl;
    class->drive = app_car_drive_impl;
}

GType app_car_get_type() {
  static GType type = 0;
  if (type == 0) {
      const GTypeInfo info = {
          sizeof(AppCarClass),
          NULL, /* base_init */
          NULL, /* base_finalize */
          (GClassInitFunc) AppCarClassInit,  /* class_init */
          NULL, /* class_finalize */
          NULL, /* class_data */
          sizeof(AppCar),
          0,    /* n_preallocs */
          NULL, /* instance_init */
          NULL, /* value_table */
      };
      type = g_type_register_static(G_TYPE_OBJECT, "AppCar", &info, 0);
  }
  return type;
}



AppCar* app_car_new(const gchar* make, const gchar* reg_no) {
    AppCar* car = APP_CAR(g_object_new(APP_TYPE_CAR, NULL));
    car->make = (gchar*) make;
    car->reg_no = (gchar*) reg_no;
    car->driven_kms = 0;
    return car;
}

void app_car_drive(AppCar* self, gint num_kms) {
    AppCarClass *klass;

    klass = APP_CAR_GET_CLASS(self);
    g_return_if_fail(klass->drive != NULL);

    klass->drive(self, num_kms);
}

void app_car_display(AppCar* self) {
    AppCarClass *klass;

    klass = APP_CAR_GET_CLASS(self);
    g_return_if_fail(klass->display != NULL);

    klass->display(self);
}

void app_car_display_impl(AppCar* self) {
    g_printf("AppCar*: make: %s, reg_no %s, driven: %d kms\n",
             (char*) self->make,
             (char*) self->reg_no,
             (int) self->driven_kms);
}

void app_car_drive_impl(AppCar* self, gint num_kms) {
    self->driven_kms += num_kms;
}