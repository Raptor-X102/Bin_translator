#define DEFINE_INSTRUCTION_FUNCS(name) \
static inline void name##_XREG_XREG(Dynamic_array* array, int reg_dst, int reg_src) { \
    INSERT_BYTE(array, PREFIX_SSE2); \
    INSERT_BYTE(array, REX_W | ((reg_dst >= 8) ? REX_R : 0) | ((reg_src >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, ESCAPE_BYTE); \
    INSERT_BYTE(array, name##_OP); \
    INSERT_BYTE(array, MOD_R | ((reg_dst & 7) << 3) | (reg_src & 7)); \
    INSTR_DEBUG_PRINTF( #name " %s, %s = ", X_Registers_names[reg_dst], X_Registers_names[reg_src]);\
    INSTR_DEBUG_OUT_LOOP(array, 5)\
} \
\
static inline void name##_XREG_MEM(Dynamic_array* array, int reg, int mem_reg, int32_t disp) { \
    INSERT_BYTE(array, PREFIX_SSE2); \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0) | ((mem_reg >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, ESCAPE_BYTE); \
    INSERT_BYTE(array, name##_OP); \
    INSERT_BYTE(array, MOD_DISP32 | ((reg & 7) << 3) | (mem_reg & 7)); \
    INSERT_DWORD(array, disp); \
    INSTR_DEBUG_PRINTF( #name " %s, [%s + %x] = ", X_Registers_names[reg], X_Registers_names[mem_reg], disp);\
    INSTR_DEBUG_OUT_LOOP(array, 9)\
}
