#ifndef TOOLS_H
#define TOOLS_H

#include <unistd.h>


void die(const char* msg);

void* xcalloc(size_t nmembers, size_t size);

#endif