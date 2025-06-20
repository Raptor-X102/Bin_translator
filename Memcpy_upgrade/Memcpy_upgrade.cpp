#include "Memcpy_upgrade.h"

void Memcpy_safe(char* buffer_dst, char* buffer_src, size_t bytes_to_cpy) {

    int times_cpy = bytes_to_cpy / Max_bytes_to_cpy;
    for(int i = 0; i < times_cpy; i++)
        memcpy(buffer_dst + i * Max_bytes_to_cpy, buffer_src + i * Max_bytes_to_cpy, Max_bytes_to_cpy);

    int remain = bytes_to_cpy % Max_bytes_to_cpy;
    if(remain)
        memcpy(buffer_dst + times_cpy * Max_bytes_to_cpy, buffer_src + times_cpy * Max_bytes_to_cpy, remain);
}
