#define DEFINE_INSTRUCTION_FUNCS(name) \
static inline void name##_REG_REG(Dynamic_array* array, int reg_dst, int reg_src) { \
    INSERT_BYTE(array, REX_W | ((reg_dst >= 8) ? REX_R : 0) | ((reg_src >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, name##_R_RM_OP); \
    INSERT_BYTE(array, MOD_R | ((reg_dst & 7) << 3) | (reg_src & 7)); \
    INSTR_DEBUG_PRINTF( #name " %s, %s = ", Registers_names[reg_dst], Registers_names[reg_src]);\
    INSTR_DEBUG_OUT_LOOP(array, 3)\
} \
\
static inline void name##_REG_MEM(Dynamic_array* array, int reg, int mem_reg, int32_t disp) { \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0) | ((mem_reg >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, name##_R_RM_OP); \
    INSERT_BYTE(array, MOD_DISP32 | ((reg & 7) << 3) | (mem_reg & 7)); \
    INSERT_DWORD(array, disp); \
    INSTR_DEBUG_PRINTF( #name " %s, [%s + %x]= ", Registers_names[reg], Registers_names[mem_reg], disp);\
    INSTR_DEBUG_OUT_LOOP(array, 7)\
} \
\
static inline void name##_MEM_REG(Dynamic_array* array, int mem_reg, int reg, int32_t disp) { \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0) | ((mem_reg >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, name##_RM_R_OP); \
    INSERT_BYTE(array, MOD_DISP32 | ((reg & 7) << 3) | (mem_reg & 7)); \
    INSERT_DWORD(array, disp); \
    INSTR_DEBUG_PRINTF( #name " [%s + %x], %s = ", Registers_names[mem_reg], disp, Registers_names[reg]);\
    INSTR_DEBUG_OUT_LOOP(array, 7)\
} \
static inline void name##_REG_MEM_DISP0(Dynamic_array* array, int reg, int mem_reg) { \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0) | ((mem_reg >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, name##_R_RM_OP); \
    INSERT_BYTE(array, MOD_DISP0 | ((reg & 7) << 3) | (mem_reg & 7)); \
    INSTR_DEBUG_PRINTF( #name " %s, [%s] = ", Registers_names[reg], Registers_names[mem_reg]);\
    if (mem_reg == I_RSP) {\
    INSERT_BYTE(array, SIB_RSP_ONLY);\
    INSTR_DEBUG_OUT_LOOP(array, 4)}\
    else {\
    INSTR_DEBUG_OUT_LOOP(array, 3) }\
} \
static inline void name##_MEM_REG_DISP0(Dynamic_array* array, int mem_reg, int reg) { \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0) | ((mem_reg >= 8) ? REX_B : 0)); \
    INSERT_BYTE(array, name##_RM_R_OP); \
    INSERT_BYTE(array, MOD_DISP0 | ((reg & 7) << 3) | (mem_reg & 7)); \
    INSTR_DEBUG_PRINTF( #name " [%s], %s = ", Registers_names[mem_reg], Registers_names[reg]);\
    if (mem_reg == I_RSP) {\
    INSERT_BYTE(array, SIB_RSP_ONLY);\
    INSTR_DEBUG_OUT_LOOP(array, 4) }\
    else {\
    INSTR_DEBUG_OUT_LOOP(array, 3) }\
} \
\
static inline void name##_REG_IMM(Dynamic_array* array, int reg, int32_t imm32) { \
    INSERT_BYTE(array, REX_W | ((reg >= 8) ? REX_R : 0)); \
    INSERT_BYTE(array, INSRT_IMM32_OP); \
    INSERT_BYTE(array, MOD_R | (MOD_##name << 3) | (reg & 7)); \
    INSERT_DWORD(array, imm32); \
    INSTR_DEBUG_PRINTF( #name " %s, %x = ", Registers_names[reg], imm32);\
    INSTR_DEBUG_OUT_LOOP(array, 7)\
}
