# Discussion

This design builds on `attempt-02` by extending it with vtables. The object struct now holds only data members, as well as a pointer to a vtable struct.

```c
struct Object {
    const ObjectVTable* vtable;
    const uint64_t typeid;
    const uint64_t objid;
};
```

The vtable holds all the function pointers:

```c
struct ObjectVTable {
    const Object_Destroy destroy;
    const Object_Display display;
    const Object_GetObjId get_objid;
    const Object_SubclassPrint subclass_print;
};
```

The method call syntax has now become:

```c
instance->vtable->display(instance);
```

---

A subclass will mirror this organization, thus it will have its own object struct and its own vtable. The subclass vtable holds only the methods specific to the subclass. As before, the `super` member nests the superclass struct within the subclass struct:

```c
struct Car {
    const Object super;

    const CarVTable* vtable;
    const char* const make;
    const char* const reg_no;
    int driven;
};

struct CarVTable {
    const Car_Drive drive;
    const Car_Print print;
};
```


## Object construction

Constructing an object now requires populating both the object struct and the vtable struct. The vtable struct will be the same for all instances of a class, so it can be a static object defined at file level:

```c
const ObjectVTable Object_vtable = {
    .destroy = Object_destroy,
    .display = Object_display,
    .get_objid = Object_getObjId,
    .subclass_print = Object_subclassPrint,
};
```

The object struct is dynamically allocated and populated much like before, but now contains a pointer to the vtable struct:

```c
Object instance = {
    .vtable = &Object_vtable,
    .typeid = TYPEID_OBJECT,
    .objid = rand(),
};
```

---

For a subclass like `Car` the same applies, except it must also populate the vtable that belongs to its superclass. This is quite necessary, otherwise it would have no way to overload methods from the superclass:

```c
const ObjectVTable Object_vtable = {
    .destroy = (Object_Destroy) Car_destroy,
    .display = (Object_Display) Car_display,
    .get_objid = Object_getObjId,
    .subclass_print = Object_subclassPrint,
};

const CarVTable Car_vtable = {
    .drive = Car_drive,
    .print = Car_print,
};
```

With the vtables set up we can create the object struct itself:

```c
Car instance = {
    .super = {
        .vtable = &Object_vtable,
        .typeid = TYPEID_CAR,
        .objid = rand(),
    },

    .vtable = &Car_vtable,
    .make = xstrdup(make),
    .reg_no = xstrdup(reg_no),
    .driven = 0,
};
```


## Accessing data

Let's assume we have an instance of `Car`. If we upcast it to `Object*` then we can call any of `Object`'s methods and those methods will find the data members defined by `Object` no problem:

```c
Car* car = Car_create("Saab", "ABC-123");
Object* object = (Object*) car;
uint64_t objid = object->vtable->get_objid(object);

uint64_t Object_getObjId(Object* instance) {
    return instance->objid;
}
```

Likewise, if we want to call a method on `Car` we can do that without having to do anything special. The method we call will have access to the data members that `Car` defines.

```c
car->vtable->drive(car, 2);

void Car_drive(Car* instance, int num_kms) {
    instance->driven += num_kms;
}
```

If the `Car` method wants to access a data member defined by `Object` it can, but it needs to know that it's part of the superclass, not `Car` itself:

```c
void Car_something(Car* instance) {
    instance->super.objid;
}
```


## Inheritance and methods

One question we'd like to ask is: what happens if a subclass wants to call a method on a superclass? Or vice versa?

Though contrived we've added some methods to be able to show how this works. The chain of method calls is:

```c
(superclass) subclass_print -> (subclass) print -> (superclass) get_objid
```

Once again, we have a `Car` object in scope. We upcast it to `Object*`, and we call a method that belongs to the superclass:

```c
Object* instance = (Object*) car;
instance->vtable->subclass_print(instance);
```

We want this method to call a method on the subclass. This is not guaranteed to work, because what if `instance` is just a plain `Object*`? We have to do a checked cast to see if it's possible to call the subclass method:

```c
void Object_subclassPrint(Object *instance) {
    Car* car = Car_cast(instance);
    if (car) {
        car->vtable->print(car);
    }
}
```

The subclass method will in turn call a method (a different one!) on the superclass, just to show how that works:

```c
void Car_print(Car* instance) {
    uint64_t objid = instance->super.vtable->get_objid((Object*) instance);
    printf("Printing car with objid: %lu\n", objid);
}
```

The method invocation we see here is pretty convoluted: the subclass has to know to go through `super` to reach the superclass struct, and then go through `vtable` to find the superclass vtable.