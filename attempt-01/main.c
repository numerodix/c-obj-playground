#include "object.h"
#include "tools.h"


int main() {
    Object* instance = Object_create();

    instance->display(instance);
    instance->destroy(instance);

    return 0;
}