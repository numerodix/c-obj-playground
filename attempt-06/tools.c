#include "tools.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


void die(const char* msg) {
    fprintf(stderr, "program aborted with: %s\n", msg);
    _exit(1);
}

void* xcalloc(size_t nmembers, size_t size) {
    void *block = calloc(nmembers, size);

    if (!block) {
        die("calloc returned NULL");
    }

    return block;
}

int xrandint(int min, int max) {
    return (int) (
        (double) min + 
        (((double) (max - min) * (double) rand()) / (double) RAND_MAX)
    );
}

char* xstrdup(const char *src) {
    char *dest = strdup(src);

    if (!dest) {
        die("strdup returned NULL");
    }

    return dest;
}