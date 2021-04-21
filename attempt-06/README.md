# Discussion

In `attempt-06` we're aiming for an object system that resembles C++ more closely.


## Taking stock of where we are

Ever since `attempt-02` onwards we've been in a world where we rely on struct nesting to express inheritance, both in object structs and in class/vtable structs. This has the advantage that the superclass struct is easily included by the subclass and we achieve some level of encapsulation this way. But it also has the major disadvantage that the class hierarchy inexorably is part of the programming model. When accessing an object member we always need to know which class it's in:

```c
ecar->charge_kwhs;          // charge_kwhs is a member of EletricCar
ecar->super.driven_kms;     // driven_kms is a member of Car
ecar->super.super.objid;    // objid is a member of Object
```

And when calling a method on a superclass we also need a cast because the vtable pointer is of type `ObjectClass*`, ie. the top most superclass. In order to access the methods in _our_ object's vtable we need to cast it to the corresponding class first:

```c
// calling a method on ElectricCar itself
((const ElectricCarVTable*) ecar->super.super.vtable)->charge(ecar, 7);
// calling a method on Car
((const ElectricCarVTable*) ecar->super.super.vtable)->super.drive((Car*) ecar, 2);
// calling a method on Object
((const ElectricCarVTable*) ecar->super.super.vtable)->super.super.display((Object*) ecar);
```

Now granted a certain amount of this field traversal and casting can be hidden using macros, but that just introduces a new vocabulary that the programmer has to use. It's not ideal if we're aiming for an object system in plain C.


## A new beginning

This time we'll start with the user experience first and work our way into the guts of the system later.

Behold, creating objects of different classes is simple:

```c
Object* object = Object_create();
Car* saab = Car_create("Saab", "ABC-123", 10);
Car* volvo = Car_create("Volvo", "SEK-977", 10);
ElectricCar* tesla = ElectricCar_create("Tesla", "EVP-400", 10, 50);
```

Passing objects of different classes to the same function doesn't require a cast:

```c
display_and_delete(object);
display_and_delete(saab);
display_and_delete(tesla);
```

For this to work the function must expect `void*` because that's the only pointer type C will allow a transparent conversion *to*, without a cast:

```c
void display_and_delete(void* instance) {
```

The receiving function will need to cast, but it can do relatively convenient checked casts before calling methods on the objects:

```c
void display_and_delete(void* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        car->vtable->drive(car, xrandint(1, 10));
    }

    ElectricCar* ecar = ElectricCar_cast(instance);
    if (ecar) {
        ecar->vtable->charge(ecar, xrandint(100, 200));
    }

    Object* object = Object_cast(instance);
    if (object) {
        object->vtable->display(object);
        object->vtable->delete(object);
    }
}
```

Importantly, calling methods on an object has the same syntax regardless of the class the methods belong to, and doesn't require a cast:

```c
// calling a method on ElectricCar itself
ecar->vtable->charge(ecar, xrandint(100, 200));
// calling a method on Car
ecar->vtable->drive(ecar, 2);
// calling a method on Object
ecar->vtable->display(ecar);
```

And accessing fields on an object has the same syntax no matter which class the field comes from:

```c
ecar->charge_kwhs;   // charge_kwhs is a member of EletricCar
ecar->driven_kms;    // driven_kms is a member of Car
ecar->objid;         // objid is a member of Object
```

That's all the syntax for dealing with objects from the outside!

---

Next up, writing methods.

`Object_delete` expects an `Object*`:

```c
void Object_delete(Object* self) {
    free(self);
}
```

And `Car_delete` expects a `Car*`, even though this is an overloaded method that belongs to `Object`.

```c
void Car_delete(Car* self) {
    free((char*) self->make);
    free((char*) self->reg_no);
    free(self);
}
```

In other words, `Object_delete` and `Car_delete` are *different types*. Compatible types, but different. This is why the caller doesn't have to cast the pointer at the call site, which is a major usability win!

Methods from a subclass can access data members and call methods from a superclass without needing to know anything about the class hierarchy:

```c
void ElectricCar_display(ElectricCar* self) {
    printf("ElectricCar* at: %p, objid: %d -- "
           "make: %s, reg_no: %s, driven: %d kms, charge: %d kwhs\n",
           (void*) self,
           // get_objid belongs to Object
           self->vtable->get_objid(self),
           // these all belong to Car
           self->make, self->reg_no, self->driven_kms,
           // charge_kwhs belongs to ElectricCar
           self->charge_kwhs);
}
```

Okay, this feels a lot more like an object oriented programming model, doesn't it?


## Object representation

Representation really is the crux of the matter, because we can't achieve a flat namespace for data members and methods without having flat (ie. non-nested) structs!

We go back to basics to where we started in `attempt-01`:

```c
struct _Object {
    ObjectVTable* vtable;
    int objid;
};

struct _Car {
    CarVTable* vtable;
    int objid;
    const char* make;
    const char* reg_no;
    int driven_kms;
};
```

To cut down on the copy pasting we can move the fields into macros instead:

```c
#define ELECTRIC_CAR_FIELDS \
    int charge_kwhs;

struct _ElectricCar {
    ElectricCarVTable* vtable;
    OBJECT_FIELDS
    CAR_FIELDS
    ELECTRIC_CAR_FIELDS
};
```

This is less bad, because we're not forcing macros on the *users* of the class, just the implementors.

There's one very important point here, which is that the type of the vtable pointer matches the type of the class:

```c
struct _Object {
    ObjectVTable* vtable;
    ...

struct _Car {
    CarVTable* vtable;
    ...

struct _ElectricCar {
    ElectricCarVTable* vtable;
    ...
```

This is different from what we saw before, and this is how we avoid having to cast the vtable pointer when calling methods!

---

As far as vtables go, they mirror this pattern of subclassing by adding methods to the vtable without nesting:

```c
struct _ObjectVTable {
    void* super;  // never points to anything!
    Object_Delete delete;
    Object_Display display;
    Object_Get_Objid get_objid;
};

struct _CarVTable {
    ObjectVTable* super;
    Car_Delete delete;
    Car_Display display;
    Car_Get_Objid get_objid;
    Car_Drive drive;
};
```

But notice that `Object_Delete` is a different type from `Car_Delete` - the former expects `Object*` while the latter expects `Car*`:

```c
typedef void (*Object_Delete)(Object* self);
typedef void (*Car_Delete)(Car* self);
```

This again avoids casting at the call site, because to call `car->vtable->delete(car)` the method already expects `Car*`!

And to aid with down casting the vtables are chained together:

```c
struct _ObjectVTable {
    void* super;  // never points to anything!
    ...

struct _CarVTable {
    ObjectVTable* super;
    ...

struct _ElectricCarVTable {
    CarVTable* super;
    ...
```


## Object construction

Our vtables are static objects as before. They need to be chained in inheritance order and they need to be populated with method pointers. This is almost equivalent to what we saw in earlier attempts, except that for an `ElectricCarVTable` all the method pointers are of a type that expects `EletricCar*`. If we want to inherit a method from a superclass we can, but we need to cast it to an `ElectricCar_Something`. This is effectively one of very few places where casts are necessary:

```c
ElectricCarVTable ElectricCar_vtable = {
    .super = &Car_vtable,
    .delete = (ElectricCar_Delete) Car_delete,
    .display = ElectricCar_display,
    .get_objid = (ElectricCar_Get_Objid) Object_get_objid,
    .drive = (ElectricCar_Drive) Car_drive,
    .charge = ElectricCar_charge,
};
```

Constructing objects is done using a class specific factory function that takes the right parameters for that class. To allow reuse each class provides an init function to initialize its fields:

```c
Car* Car_create(const char* make, const char* reg_no, int driven_kms) {
    Car* car = (Car*) xcalloc(1, sizeof(Car));
    Object_init((Object*) car);
    Car_init(car, make, reg_no, driven_kms);
    car->vtable = &Car_vtable;
    return car;
}

void Object_init(Object* self) {
    self->objid = xrandint(1, 1000);
}

void Car_init(Car* self, const char* make, const char* reg_no, int driven_kms) {
    self->make = xstrdup(make);
    self->reg_no = xstrdup(reg_no);
    self->driven_kms = driven_kms;
}
```


## Checked casts

And finally, each class must provide a function to cast to its own type. This function is the same for every class, apart from the types used in the internal casts that it needs to do. What it does is walk the chained vtables to see if the object that was passed in can reach the vtable in question (`Car_vtable` in this case), and if so, returns the object cast to that type (`Car*`).

```c
Car* Car_cast(void* instance) {
    CarVTable* vtable = ((Car*) instance)->vtable;

    while (vtable) {
        if (vtable == &Car_vtable) {
            return (Car*) instance;
        }
        vtable = (CarVTable*) vtable->super;
    }

    return NULL;
}
```

This code is a bit tricky, because it looks like we expect `instance` to be `Car*` when we cast it on the first line. But it will work on *any* object because all objects contain a `vtable` field at the top of their struct, just like `Car` does.

We then pretend that the vtable is of type `CarVTable*`, but again it could just as well be `ObjectVTable*` or some other class. We do this because we just cast `instance` to `Car*` earlier and that means its `vtable` *must* be a `CarVTable`.

We compare the vtable pointer we found against `Car_vtable` (ie. `Car`'s actual vtable). If it's a match we know that `instance` is a `Car*`.

Otherwise we keep walking the chain. And again we pretend that the superclass vtable is a `CarVTable*` because the variable `vtable` already has that type.

So of all the locations in the system this one function is where we've accumulated most of the casts. :)


## Conclusions

We've achieved a design that - relative to all our earlier attempts - looks a lot more like C++, or indeed any familiar object oriented language.
