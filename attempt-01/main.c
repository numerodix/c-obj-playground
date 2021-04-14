#include "object.h"
#include "tools.h"


int main() {
    Object* instance_fst = Object_create();
    Object* instance_snd = Object_create();

    instance_fst->display(instance_fst);
    instance_snd->display(instance_snd);

    instance_fst->destroy(instance_fst);
    instance_snd->destroy(instance_snd);

    return 0;
}