# Discussion

In 2012 there was a revamp of the object system in `htop`. Function pointers were moved from the object struct into a vtable struct. This design is still intact today in 2021. What we do in `attempt-04` is try to extract the principles of that design and apply them while evolving what we had in `attempt-02`, while keeping all of the same invariants in place.

