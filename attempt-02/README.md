# Discussion

This design is closely based on the object system in earlier versions of `htop` (from 2006).

It's mostly the same as `attempt-01`. The main difference is that we are nesting structs as a way to represent subclassing, rather than copying the fields from the superclass:

```c
struct Car {
    // inject fields that belong to Object
    const Object super;

    // add fields that belong only to Car
    const Car_Drive drive;
    const char* const make;
    const char* const reg_no;
    int driven;
};
```

This has the interesting effect that we get a nested struct to program against:

```c
Car* car = Car_create("Saab", "ABC-123");

// method that belongs to Object
car.super->display(car);

// method that belongs to Car
car->drive(car, 2);
```

However, in terms of memory layout it's still true (as before) that we have a flat layout in which the fields that belong to `Object` come first and the fields that belong only to `Car` come afterwards. This means that we can upcast from `Car*` to `Object*` as we did before and it still works:

```c
Object* object = (Object*) car;
object->display(object);
```

When creating the object we now see the struct nesting reflected in the code as well:

```c
Car instance = {
    .super = {
        .destroy = (Object_Destroy) Car_destroy,
        .display = (Object_Display) Car_display,
        .typeid = TYPEID_CAR,
        .objid = rand(),
    },

    .drive = Car_drive,
    .make = xstrdup(make),
    .reg_no = xstrdup(reg_no),
    .driven = 0,
};
```


## Class hierarchy

We've added another subclass in the hierarchy to better illustrate how inheritance currently works. The hierarchy is now:

`Object -> Car -> ElectricCar`


## Upcasting and downcasting

Upcasting any instance to `Object*` is always safe, by design.

Upcasting from `ElectricCar*` to `Car*`, as well as any kind of downcasting from `Object*` down the hierarchy is supported through the use of type ids, as we saw before:

```c
void func(Object *object) {
    // checked downcast
    ElectricCar* ecar = ElectricCar_cast(object);

    // checked upcast
    Car *car = Car_cast(ecar);
}
```

In theory, upcasts don't need to be checked because a user would always know that `Car` is a superclass of `ElectricCar`. But in large type hierarchies this may not always be so obvious and using a checked cast would prevent bugs.

In order to make the check work the superclass does actually need to know about all of its subclasses:

```c
Car* Car_cast(Object* instance) {
    if ((instance->typeid != TYPEID_CAR) &&
        (instance->typeid != TYPEID_ELECTRIC_CAR)) {
        return NULL;
    }

    return (Car*) instance;
}
```


## Inheriting methods

Another thing that we expect from a class system is that when we're implementing a subclass we should have a way to inherit method implementations from the superclass.

This is supported because object creation is explicit struct definition:

```c
ElectricCar instance = {
    // Car
    .super = {
        // Object
        .super = {
            // we can use Car_destroy directly because ElectricCar doesn't
            // add any new struct fields that need to be free'd
            .destroy = (Object_Destroy) Car_destroy,
            .display = (Object_Display) ElectricCar_display,
            .typeid = TYPEID_ELECTRIC_CAR,
            .objid = rand(),
        },

        // the drive method is just reusing Car's drive() so no need for a
        // cast here
        .drive = Car_drive,
        .make = xstrdup(make),
        .reg_no = xstrdup(reg_no),
        .driven = 0,
    },

    .charge = ElectricCar_charge,
    .charge_kwhs = charge_kwhs,
};
```

As we see here, `ElectricCar` can choose between:

1. Overloading the method as we do with `ElectricCar_display`.
2. Inheriting the method from `Car` as we do with `Car_destroy` and `Car_drive`.
3. Inheriting the method from `Object` (not shown but possible).
