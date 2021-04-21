#ifndef TOOLS_H
#define TOOLS_H

#include <unistd.h>


void die(const char* msg);

void* xcalloc(size_t nmembers, size_t size);
int xrandint(int min, int max);
char* xstrdup(const char *src);

#endif