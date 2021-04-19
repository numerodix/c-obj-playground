#ifndef CAR_H
#define CAR_H

#include <glib-object.h>


typedef struct _AppCar AppCar;
typedef struct _AppCarClass AppCarClass;

typedef void (*AppCar_Display)(AppCar* self);
typedef void (*AppCar_Drive)(AppCar* self, gint num_kms);

struct _AppCar {
    GObject parent;
    gchar* make;
    gchar* reg_no;
    gint driven_kms;
};

struct _AppCarClass {
    GObjectClass parent;
    AppCar_Display display;
    AppCar_Drive drive;
};

GType app_car_get_type (void);

#define APP_TYPE_CAR app_car_get_type ()
#define APP_CAR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), APP_TYPE_CAR, AppCar))
#define APP_CAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), APP_TYPE_CAR, AppCarClass))
#define APP_IS_CAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE(obj, app_car_get_type()))

AppCar* app_car_new(const gchar* make, const gchar* reg_no);
void app_car_display(AppCar* self);
void app_car_drive(AppCar* self, gint num_kms);

void app_car_display_impl(AppCar* self);
void app_car_drive_impl(AppCar* self, gint num_kms);

#endif