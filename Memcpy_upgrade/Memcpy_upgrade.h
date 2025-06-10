#ifndef MEMCPY_UPGRADE
#define MEMCPY_UPGRADE

#include <string.h>
#include <stdio.h>

const int Max_bytes_to_cpy = 1024;
void Memcpy_safe(char* buffer_dst, char* buffer_src, size_t bytes_to_cpy);
#endif
