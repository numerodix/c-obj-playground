# Discussion

In 2012 there was a revamp of the object system in `htop`. Function pointers were moved from the object struct into a vtable struct. This design is still intact today in 2021. What we do in `attempt-04` is try to extract the principles of that design and apply them while evolving what we had in `attempt-02`, while keeping all of the same invariants in place.

If we start with the object structs first, `Object` is special because it's the top of the class hierarchy. It holds a pointer to the vtable and not much else:

```c
struct Object {
    const ObjectVTable* vtable;
    const uint64_t objid;
};
```

Every subclass of `Object` contains `Object` inside the field `super` and then adds its own fields below it:

```c
struct Car {
    const Object super;
    const char* const make;
    const char* const reg_no;
    int driven;
};
```

Next come the vtable structs. The top of the class hierarchy is called `ObjectVTable` and contains the methods that we want all objects (not just instances of `Object` but instances of _any_ class) to have:

```c
struct ObjectVTable {
    const ObjectVTable* extends;
    const Object_Destroy destroy;
    const Object_Display display;
};
```

Importantly, here we also have the pointer `extends` which allows us to chain vtables in the hierarchy together. This will become important later when we're doing downcasting.

The subclass of `Object` will have its own corresponding vtable which contains the new methods introduced for that subclass. It will also, as its first field called `super`, contain the superclass vtable:

```c
struct CarVTable {
    const ObjectVTable super;
    const Car_Drive drive;
};
```

To get an idea of how all of these structs work together here's a picture of what a `Car` instance looks like at runtime:

```
                                    ---- Object_vtable -----  <----\
                                    | - *extends           |       |
                                    | - destroy            |       |
                                    | - display            |       |
                                    ------------------------       |
                                                                   |
---------- car ---------                                           |
| - super --\          |            ------ Car_vtable ------       |
|            |         |            | - super --\          |       |
|     --------------   |            |            |         |       |
|     | - *vtable  |   |  ------>   |     --------------   |       |
|     | - objid    |   |            |     | - *extends |   |  -----/
|     --------------   |            |     | - destroy  |   |
|                      |            |     | - display  |   |
| - make               |            |     --------------   |
| - reg_no             |            |                      |
| - driven             |            | - drive              |
------------------------            ------------------------
```

On the left hand side we have the variable `car` which is a `Car*`. It contains the fields `make`, `reg_no` and `driven`, as well as the field `super` which is a nesting of the `Object` struct. That's where we find the vtable pointer.

The vtable pointer points to the struct `Car_vtable`, which is an instantiation of `CarVTable`. This struct holds the method `drive` which is specific to cars, as well as the field `super` which nests the struct `ObjectVTable`. This allows `car` to access not only `drive()` but also `destroy()` and `display()`.

Finally, the nested `ObjectVTable` struct has a field called `extends` which encodes the class hierarchy relationship. `extends` is a pointer to `Object_vtable`, which is an instance of `ObjectVTable`. In other words, `Car_vtable` actually has *two* ways to access the superclass vtable, which seems redundant (and it is). But having the `extends` pointer solves the problem of being able to iterate up the class hierarchy when we need to upcast a pointer. This obviates the need for type ids which we've been using thus far.


## Object construction

Construction is an iteration on what we saw in `attempt-03`. A new class first has to construct the vtable struct, which is going to be a static object. As we know, `Object` is a special case here:

```c
const ObjectVTable Object_vtable = {
    .extends = NULL,  // points at nothing
    .destroy = Object_destroy,
    .display = Object_display,
};
```

Next we will construct the instance and point it at the vtable:

```c
Object instance = {
    .vtable = &Object_vtable,
    .objid = rand(),
};
```

---

For a subclass the vtable will contain *all the superclass vtables*. This is more apparent lower down the hierarchy, so we're showing `ElectricCar` here:

```c
const ElectricCarVTable ElectricCar_vtable = {
    // CarVTable
    .super = {
        // ObjectVTable
        .super = {
            // points at the immediate superclass
            .extends = (const ObjectVTable*) &Car_vtable,
            // inherit this method from the immediate superclass
            .destroy = (Object_Destroy) Car_destroy,
            // overload this method
            .display = (Object_Display) ElectricCar_display,
        },

        // inherit this method from the superclass
        .drive = Car_drive,
    },

    // ElectricCar's own methods
    .charge = ElectricCar_charge,
};
```

Whew, that's convoluted isn't it! But if we ignore the struct nesting and the casting then what we're looking at is really just a method table for a subclass where we're deciding which methods to overload and which to inherit:

```c
// the same but in pseudo code:
const ElectricCarVTable ElectricCar_vtable = {
    // ObjectVTable
    .extends = &Car_vtable,
    .destroy = Car_destroy,
    .display = ElectricCar_display,

    // CarVTable
    .drive = Car_drive,

    // ElectricCar's own methods
    .charge = ElectricCar_charge,
};
```

Constructing the object struct is equivalently nested but somewhat simpler:

```c
ElectricCar instance = {
    .super = {  // Car
        .super = {  // Object
            .vtable = (const ObjectVTable*) &ElectricCar_vtable,
            .objid = rand(),
        },
        .make = xstrdup(make),
        .reg_no = xstrdup(reg_no),
        .driven = 0,
    },
    .charge_kwhs = charge_kwhs,
};
```

There is one crucial thing here, namely that the `vtable` pointer for an `ElectricCar`, even though it sits inside the nested `Object` struct, must point at `ElectricCar_vtable`! (And yes, a cast is needed to allow this.)


## Programming API

The capabilities of the system are still the same and our example program still works. We can still construct objects of different classes and pass them to the same function, upcasting in the process:

```c
int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123");
    ElectricCar* tesla = ElectricCar_create("Tesla", "REY-570", 50);

    display_and_destroy(object);
    display_and_destroy((Object*) saab);
    display_and_destroy((Object*) tesla);
}
```

What has changed a lot is the method call syntax. For objects of `Object` it's actually very simple:

```c
void display_and_destroy(Object* instance) {
    instance->vtable->display(instance);
}
```

But downcasting has become a lot more cumbersome:

```c
if (Object_is_A(instance, (const ObjectVTable*) &Car_vtable)) {
    Car* car = (Car*) instance;
```

If we want to know if `instance` is a `Car*` then we first need to call `Object_is_A` with the argument `Car_vtable`. What this will do is walk the class hierarchy of `instance` to see whether we encounter `Car_vtable` at some point along the way. If we do, that means `instance` is-a `Car`.

At that point we know that casting `instance` to `Car*` is safe, so we can do that. Voilà, we now have a `Car*`. This is the equivalent of the checked cast we had in earlier attempts, but without having to rely on type ids.

Alright, but now we're in for a disappointment. Calling a method on `car` is not going to be as easy as we'd hope:

```c
    int howfar = rand();
    ((const CarVTable*) car->super.vtable)->drive(car, howfar);
}
```

Okay, what's going on here? Well, as you will recall we have a vtable pointer somewhere in this `Car` struct. But it's sitting inside the `super` field, which is a nested `Object` struct. Now, according to the `Object` struct the field `vtable` is a `const ObjectVTable*`. So once we do `car->super.vtable` all we can access are methods that belong to `Object`. Except we know that this vtable is *actually* a `CarVTable`, don't we? I mean, it has to be, because `car` is-a `Car`! So now we have to cast this value to `const CarVTable*` in order to *see* the methods that belong to `Car`. (Or if you prefer, in order for the C language to give us access to those methods.)

If that rationale didn't make you jump for joy, then I'm afraid it only gets worse from here. Having a `Car` and trying to call a method specific to `Car` is the happy case. Calling a method from a superclass is actually worse, and I'll show you why.

Let's say instead we have an `ElectricCar`. We want to call the method `drive()` which belongs to `Car`. What do we do?

```c
((const ElectricCarVTable*) ecar->super.super.vtable)->super.drive((Car*) ecar, 2);
```

The idea is the same, but with more steps:

1. Walk the nested `super` fields until we find the vtable.
2. Cast the vtable to the class that we know this object is, ie. `ElectricCarVTable*`.
3. Step down into the first nested `super` field to find the methods that belong to `Car`.
4. Call the `drive()` method. Because `ecar` is an `ElectricCar*` we need to cast it to `Car*`.

I guess you might be wondering: how does `htop` deal with this inconvenient syntax? In a way, it doesn't because all the casting and `super` field traversals are hidden using macros. The complexity is there, but with syntactic sugar on top. 