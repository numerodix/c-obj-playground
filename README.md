# Experiments with object systems

We've all seen C APIs that work like this:

1. Define a struct that will store the user's state.
2. Define functions that operate on the struct. This includes life cycle functions to allocate and free the struct.
3. We can even make the struct an abstract type by forward declaring it in the header, but not providing a definition (in the header). This ensures that the user can only interact with the struct through the functions that we provide.

Okay, but what if we wanted to take this concept further and turn it into an object system where the struct is an object, it belongs to a certain class and that class provides certain methods (stored in the struct as function pointers), classes can inherit from each other and so forth? How would we do that?

Well, that's exactly the question we are exploring in this project. The project consists of a series of attempts at an object system, with working code and a discussion of the benefits and drawbacks of each attempt. These attempts tell a story, so we suggest reading them in order.

1. [attempt-01 - Simplest design I could think of](attempt-01)
2. [attempt-02 - Early htop](attempt-02)
3. [attempt-03 - First attempt at vtables](attempt-03)
4. [attempt-04 - Present day htop](attempt-04)
4. [attempt-05 - Implemented in GLib](attempt-05)

For each attempt you'll want to do:

```bash
# to run the code
$ make run

# to check for memory safety problems
$ make check
```

This exploration is inspired by looking at these projects:

* [glib](https://gitlab.gnome.org/GNOME/glib)
* [htop](https://github.com/htop-dev/htop)
