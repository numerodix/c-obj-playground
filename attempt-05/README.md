# Discussion


## Quick start

NOTE: This attempt is not from the ground up like the others, it's coded against GLib. You'll need these packages to build and run it:

* `pkg-config`
* `libglib2.0-dev`


## GLib's object system

GLib is a library with big ambitions. It aims to encapsulate the C language more so than just supplement it with a few building blocks. This is apparent already from the surface syntax where GLib provides `typedef`s for primitive types: `gint` for `int`, `gpointer` for `void*` and so on. GLib wants the programmer to think in terms of GLib rather than in terms of ANSI C.

The object system in GLib is much more extensive and complex than what we've seen so far. It was designed primarily to facilitate language bindings and make objects in GLib accessible from Perl, Python and others. This is what enables developers to write Gtk/Gnome programs using a range of different languages (both compiled languages with static type systems and interpreted languages with dynamic type systems!).


## Object representation

Objects are represented more or less the same way we saw in `attempt-04`.

Let's start with the class struct and unwind it to see where it takes us. Classes in GObject are expected to include the `GObjectClass` struct as their first member:

```c
struct AppCarClass {
    GObjectClass parent;
    AppCar_Display display;
    AppCar_Drive drive;
};
```

If we pull back the curtain we can see that `GObjectClass` contains a bunch of fields, but the first of them is something called a `GTypeClass`:

```c
struct GObjectClass
{
  GTypeClass g_type_class;
  ...
```

What's `GTypeClass`? It's a struct with a single field of type `GType`:

```c
struct GTypeClass
{
  GType g_type;
};
```

What's that, you ask? `GType` is just an integer type. GLib keeps a mapping of `GType -> GTypeClass` at runtime, in other words each class in the object system gets assigned a unique integer identifier, and these can be queried at runtime. Remember how we had type ids in earlier attempts? It's the same idea here.

Let's move on to object structs next. Every object struct is expected to contain a field of type `GObject` as its first member:

```c
struct AppCar {
    GObject parent;
    gchar* make;
    gchar* reg_no;
    gint driven_kms;
};
```

`GObject` has a few fields, but its first field is something of type `GTypeInstance`:

```c
struct GObject
{
  GTypeInstance g_type_instance;
  ...
```

And a `GTypeInstance` is a struct with a single field of type `GTypeClass*`:

```c
struct GTypeInstance
{
  GTypeClass *g_class;
};
```

When an object is created this `g_class` pointer is set to its corresponding class struct. This is precisely the pattern we saw in `attempt-04`.

To summarize what we've discovered:

* `GObjectClass` is the base struct for classes. It contains a `GTypeClass`.
* `GObject` is the base struct for objects. It contains a `GTypeInstance`.
* `GTypeClass` represents a type in GLib's dynamic type system.
* `GTypeInstance` represents an instance of a type in GLib's dynamic type system and always points to a struct that is, or that contains, `GTypeClass`.
* `GType` represents a type id.

---

What about subclassing, how does that work? You can probably see this coming - the object struct contains its superclass object struct as its first field:

```c
struct AppElectricCar {
    AppCar parent;
    gint charge_kwhs;
};
```

And the class struct contains its superclass class struct as its first field:

```c
struct AppElectricCarClass {
    AppCarClass parent;
    AppElectricCar_Charge charge;
};
```

At the end of the day, this representation gives us the same capabilities we saw in `attempt-04`:

1. An object like `AppElectricCar*` is castable to `GObject*`.
1. From a `GObject*` we can follow the pointer to the `GTypeClass*`.
1. A `GTypeClass*` is castable to a `GObjectClass*`, which is the base vtable in the object system.
1. The `GObjectClass*` we have arrived at is castable to an `AppElectricCarClass*`, which is the class (or vtable) for `AppElectricCar*`. This is where we find its methods.


## Object construction

Object construction in GLib is quite different to what we've seen before and can best be understood as a series of hooks that allow the programmer to control the behavior of the different steps involved in construction and destruction.

In GLib it's not just objects that are constructed and destructed dynamically, classes are too! The first time an object of a particular class is constructed is when the class itself is constructed. Conversely, when the last object of a particular class is destructed the class itself is destructed too. This is handled transparently by the object system.

Both objects and classes have the lifecycle:

1. Allocate the slice of memory where the object/class will live - "the buffer".
1. Initialize the object/class by settings up its fields: data members and methods (function pointers). If any of these fields are dynamically allocated, this is where their buffers would be allocated and initialized as well.
1. The object/class is in use.
1. Finalize the object/class by deallocating any buffers used by fields that are dynamically allocated.
1. De-allocate the buffer used by the object/class.

This scheme is further complicated by inheritance:

1. When a class struct is initialized the portion of the struct that belongs to its superclass has to be initialized too.
1. When a class is finalized the portion of the struct that belongs to its superclass has to be finalized too.

All of these actions are encoded as functions in GLib. A class can choose to implement any these functions if it wants to customize any of the behaviors, or it can choose to inherit the default behaviors of these actions.

Let's see how this works by tracing the construction of a class. The convention in GLib is to write a custom factory function to construct an object, just like we've seen in earlier attempts:

```c
AppCar* app_car_new(const gchar* make, const gchar* reg_no) {
    AppCar* car = APP_CAR(g_object_new(APP_TYPE_CAR, NULL));
    car->make = (gchar*) make;
    car->reg_no = (gchar*) reg_no;
    car->driven_kms = 0;
    return car;
}
```

Our constructor `app_car_new` takes as parameters the items we need to set up the object data members: `make` and `reg_no`.

All the magic happens in the first statement of the function:

1. `g_object_new` is the function provided by GLib which allocates and initializes the object for us, and returns a pointer to it.
1. `APP_TYPE_CAR` is a macro that every class is expected to define. Its purpose is to return the `GType` of the class. But it does more than this, as we shall soon see.
1. `APP_CAR` is a macro that casts the pointer returned by `g_object_new`, which is a `void*`, to an `AppCar*`. This is a checked cast.

In other words, the first statement says: "dynamically allocate an `AppCar` for me, zero the whole struct, make sure its `g_class` pointer points to an `AppCarClass*`, and return a pointer to it".

But because the class may not exist yet, `APP_TYPE_CAR` actually calls the function `app_car_get_type` which not only returns the `GType` but also registers the class in the GLib type system on the first run:

```c
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
```

The struct we see here, `GTypeInfo`, is basically the paperwork we have to fill in about our new type and hand into GLib's dynamic type system. In particular, we have to tell it:

1. How big the object struct and class struct is (using `sizeof`).
1. Pointers to functions for all the hooks we want to customize. Here we see most of them are `NULL` which means we're telling GLib to use the default behavior.

With this info the GLib object/class construction machinery kicks into gear, registers this metadata about our type in its internal state, allocates a new `GType` that identifies our type, and runs all the object/class construction hooks we mentioned before.

In the case of `AppCar` the only hook we want to make use of is class initialization, so that we can set up the function pointers in the class:

```c
static void AppCarClassInit(AppCarClass* class, gpointer class_data) {
    class->display = app_car_display_impl;
    class->drive = app_car_drive_impl;
}
```

Okay, that was the first line of `app_car_new`. :) The rest is just setting fields on the new object and returning the `AppCar*`.


## Method call syntax

Given the object representation we've seen in GLib calling a method on an object would look like this:

```c
AppCar* saab = app_car_new("Saab", "ABC-123");
((AppCarClass*) saab->parent.g_type_instance.g_class)->drive(saab, 2);
```

The cast is necessary because `g_class` points to a `GTypeClass`, and we need to cast that to an `AppCarClass` in order to make `AppCar`'s own methods visible to the language.

Not surprisingly, this syntax is not what GLib recommends. Instead, the convention is to write a wrapper function which hides the field traversals and the cast, and adds some runtime type checking to make sure what we're passing as the first argument is really an `AppCar*`:

```c
app_car_drive(car, 3);

void app_car_drive(AppCar* self, gint num_kms) {
    AppCarClass *klass;

    klass = APP_CAR_GET_CLASS(self);
    g_return_if_fail(klass->drive != NULL);

    klass->drive(self, num_kms);
}
```


## Polymorphism

The final thing we want to look at is how GLib deals with polymorphism, ie. calling methods on a superclass. We construct objects of two different classes and pass them to the same function:

```c
int main() {
    AppCar* saab = app_car_new("Saab", "ABC-123");
    AppElectricCar* tesla = app_electric_car_new("Tesla", "REY-570", 50);

    display_and_delete(saab);
    display_and_delete(tesla);
}
```

The function expects a `void*` (because `gpointer` is an alias of `void*`):

```c
void display_and_delete(gpointer obj) {
    AppCar* car = APP_CAR(obj);
    app_car_drive(car, 3);

    if (APP_IS_ELECTRIC_CAR(car)) {
        AppElectricCar* ecar = APP_ELECTRIC_CAR(car);
        app_electric_car_charge(ecar, 7);
    }

    app_car_display(car);
    g_clear_object(&obj);  // decrement reference count
}
```

We know that what we're passing to the function is always castable to an `AppCar*`, but only sometimes castable to an `AppElectricCar*`. Using `APP_IS_ELECTRIC_CAR` we check if the latter cast is valid, and only call `app_electric_car_charge` on objects that are `AppElectricCar*`.

By contrast, we call `app_car_drive` and `app_car_display` on both an `AppCar*` and `AppElectricCar*` objects. This works fine. However, the GLib type system doesn't like it when we do this and issues a warning (this is part of the checked casts in GLib):

```bash
GLib-GObject-WARNING **: 12:18:33.962: invalid cast from 'AppElectricCar' to 'AppCar'
```

This is rather inconvenient, because to avoid this we'd have to re-implement `app_car_drive` as `app_electric_car_drive` and then do a case analysis on the object at runtime to decide which wrapper function to call! This tells me that GLib does not really cater to polymorphic method calls even though the design allows it.