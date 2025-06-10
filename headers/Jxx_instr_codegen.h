#define JXX_SIGNED(name) \
static inline void name##_REL32(Dynamic_array* array, int32_t disp32) { \
    INSERT_BYTE(array, COND_JMP_OP);\
    INSERT_BYTE(array, name##_REL32_OP); \
    INSERT_DWORD(array, disp32);\
    INSTR_DEBUG_PRINTF( #name " %x = ", disp32);\
    INSTR_DEBUG_OUT_LOOP(array, 6)\
}

#define JXX_UNSIGNED(name) \
static inline void name##_REL8(Dynamic_array* array, int8_t disp8) { \
    INSERT_BYTE(array, name##_REL8_OP); \
    INSERT_BYTE(array, disp8);\
    INSTR_DEBUG_PRINTF( #name " %x = ", disp8);\
    INSTR_DEBUG_OUT_LOOP(array, 2)\
}
