#define DEFINE_NASM_FUNCS(name) \
static inline void name##_NASM(Dynamic_array* array, int loc_index) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " .L");\
    _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
    int symbols_written = sprintf(array->data + array->size, "%x\n", loc_index);\
    array->size += symbols_written;\
}\
\
static inline void name##_FUNC_NASM(Dynamic_array* array, const char* func, int length) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " ");\
    _Dynamic_array_resize_if_needed(array, length);\
    INSERT_TEXT(array, func, length);\
    INSERT_STRING_CONSTANT(array, "\n");\
}
