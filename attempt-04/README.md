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

The vtable pointer points to the struct `Car_vtable`, which is an instantiation of `CarVTAble`. This struct holds the method `drive` which is specific to cars, as well as the field `super` which nests the struct `ObjectVTable`. This allows `car` to access not only `drive()` but also `destroy()` and `display()`.

Finally, the nested `ObjectVTable` struct has a field called `extends` which encodes the class hierarchy relationship. `extends` is a pointer to `Object_vtable`, which is an instance of `ObjectVTable`. In other words, `Car_vtable` actually has *two* ways to access the superclass vtable, which seems redundant (and it is). But having the `extends` pointer solves the problem of being able to iterate up the class hierarchy when we need to upcast a pointer.