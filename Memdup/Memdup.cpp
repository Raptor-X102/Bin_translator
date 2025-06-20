#include "Memdup.h"

void* memdup(const void* src, size_t n) {
    if (!src || n == 0)
        return NULL;

    void* dst = malloc(n);
    if (!dst)
        return NULL;

    memcpy(dst, src, n);

    return dst;
}
