#define DEFINE_INSTRUCTION_FUNCS(name) \
static inline void name##_REL32(Dynamic_array* array, int32_t disp32) { \
    INSERT_BYTE(array, name##_REL32_OP); \
    INSERT_DWORD(array, disp32);\
    INSTR_DEBUG_PRINTF( #name " %x = ", disp32);\
    INSTR_DEBUG_OUT_LOOP(array, 5)\
}
