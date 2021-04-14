# Discussion

The design uses a factory function to create an object and the create function can be extended with parameters as needed for the class.

```c
Object* Object_create();
Car* Car_create(const char* make, const char* reg_no);
```

The layout of object structs is such that all of them will share the same fields as `Object` at the beginning of the struct, and then add their own fields following that, so:

```c
struct Object {
    const Object_Destroy destroy;
    const Object_Display display;
    const uint64_t typeid;
    const uint64_t objid;
};

struct Car {
    // fields that belong to Object
    const Object_Destroy destroy;
    const Object_Display display;
    const uint64_t typeid;
    const uint64_t objid;

    // fields that belong only to Car (and its subclasses, potentially)
    const Car_Drive drive;
    const char* const make;
    const char* const reg_no;
    int driven;
};
```

Methods are function pointers that must first be `typedef`'d before they can be used in a struct definition:

```c
typedef void(*Object_Destroy)(Object* instance);
typedef void(*Object_Display)(Object* instance);

typedef void(*Car_Drive)(Car* instance, int num_kms);
```

The naming convention is `Class_Function` when defining the type of a function. Functions can then be defined that agree with this type, and the naming convention is `Class_function` to signal that its a member of a known function type:

```c
// matches void(*Object_Destroy)(Object* instance)
void Object_destroy(Object *instance) {
    free(instance);
}
```

Because these type signatures match such a function can thus be assigned to the corresponding struct field:

```c
Object instance = {
    .destroy = Object_destroy,  // type: Object_Destroy
    .display = Object_display,  // type: Object_Display
    .typeid = TYPEID_OBJECT,
    .objid = rand(),
};
```

When we are doing this for a subclass the types don't exactly agree and we have to do a cast:

```c
Car instance = {
    // assign: void Car_destroy(Car *instance)
    // to a:   void(*Object_Destroy)(Object* instance)
    .destroy = (Object_Destroy) Car_destroy,
    /// same here
    .display = (Object_Display) Car_display,
    ...

    // but no need for a cast here, because the types match
    .drive = Car_drive,
};
```

C allows this, perhaps because the types are "close enough":

* The return type (`void`) is the same.
* The parameter list is the same arity, and every parameter that differs is a pointer type anyway (which means its *size* is the same).

This feels hacky though.


## Const methods

One wrinkle in this design is that methods are declared `const`. This is good because it means they cannot be re-assigned once the object exists and the user has access to it;

```c
Object* instance = Object_create();
instance->destroy = something_else;   // won't compile
```

But it does mean that when constructing the object (where the goal is to return a pointer to a dynamically allocated object, ie. `Object*`) we cannot first allocate the memory and then assign the fields, because `const` doesn't allow it:

```c
Object* object = (Object*) malloc(sizeof(Object));
object->destroy = Object_destroy;  // won't compile
object->display = Object_display;  // won't compile
```

We get around this by defining the struct on the stack first, then copying its contents into the memory we just allocated dynamically:

```c
Object stack_instance = {
    .destroy = Object_destroy,
    .display = Object_display,
    .typeid = TYPEID_OBJECT,
    .objid = rand(),
};

Object* instance = (Object*) malloc(sizeof(Object));
memcpy(instance, &stack_instance, sizeof(Object));
```

This works fine because all the struct fields we are copying are just plain integers and pointers anyway, and I can't think of a case where it would not work. But it's not elegant, that's for sure.



## Polymorphism

The elementary goals of a polymorphic programming model are to be able to:

1. Construct objects of different classes.
2. Upcast them to a common base class and treat them all the same (by passing them to a function that expects instances of the base class, or storing them in a container that expects instances of the base class etc). When calling methods on these upcast objects the methods that are called *should be the methods defined in the subclass, not the superclass*.
3. Selectively downcast them back to their most specific class and call methods that only that class offers.

Properties #1 and #2 are satisfied in our model:

```c
int main() {
    Object* object = Object_create();
    Car* saab = Car_create("Saab", "ABC-123");

    generic_func(object);
    generic_func((Object*) saab);
}

void generic_func(Object* instance) {
    instance->display(instance);
    instance->destroy(instance);
}
```

We can see from the output of `instance->display()` that it's working:

```
Object_display for Object* instance
Car_display for Car* instance make: Saab reg_no: ABC-123 driven: 1714636915 kms
```

Crucially, `instance->display()` will call `Car_display` if `instance` is an `Car*` which was upcast to `Object*`. This is because both `struct Object` and `struct Car` have a `SomeClass_display` method in the same position in the struct. When the compiler generates code for this function call (via the function pointer) it will calculate the address offset and do a jump to that location. Both `Object` and `Car` will then have a function at that address whose type matches `Object_display`, and the call will work. (If that struct field were to contain something else the call would produce a program crash.)

Property #3 is also satisfied given that we give each class a unique type id. This allows us at runtime to attempt a downcast and check the type id to see if it ought to succeed or not:

```c
Car* Car_cast(Object* instance) {
    if (instance->typeid != TYPEID_CAR) return NULL;
    return (Car*) instance;
}

void generic_func(Object* instance) {
    Car* car = Car_cast(instance);
    if (car) {
        car->drive(car, 2);
    }
}
```


## Method call syntax

The syntax for calling a method is:

```c
car->drive(car, 2);
```

Obviously, the variable `car` is mentioned twice here, and that's because:

1. `car->drive` provides the address of the function to call, and
2. `car` as the first argument is needed because the method expects `Object*` as the first parameter. Otherwise it wouldn't have access to any of the object's data/method members.

This is both redundant and error prone, obviously. But there is no way around it, not even with a generic macro:

```c
#define CALL(instance, method, ...) instance->method(instance, ...)

int return_value = CALL(car, drive, 2);
```

This alternative syntax makes it less clear that we are calling a method on an object.


## Overheads

As we've seen, the object struct stores all the function pointers:

```c
struct Car {
    // fields that belong to Object
    const Object_Destroy destroy;
    const Object_Display display;
    const uint64_t typeid;
    const uint64_t objid;

    // fields that belong only to Car (and its subclasses, potentially)
    const Car_Drive drive;
    const char* const make;
    const char* const reg_no;
    int driven;
};
```

This means that every instance (ie. every car) will have a copy of every function pointer, eg. `Car_Drive`. This is quite unnecessary, because these pointers will always have the same value for every `Car` (and they *must be the same*).

We could avoid this by offloading the function pointers into a separate struct (a vtable) and just have a pointer from the object struct to the vtable struct instead:

```c
struct Car_VTable {
    const Object_Destroy destroy;
    const Object_Display display;
    const Car_Drive drive;
};

struct Car {
    const Car_VTable* const vtable;

    const uint64_t typeid;
    const uint64_t objid;

    const char* const make;
    const char* const reg_no;
    int driven;
};
```

What would this mean for the method call syntax? It would have to become:

```c
car->vtable->drive(car, 2);
```

