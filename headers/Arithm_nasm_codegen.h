#define DEFINE_NASM_FUNCS(name) \
static inline void name##_REG_REG_NASM(Dynamic_array* array, int reg_dst, int reg_src) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " "); \
    INSERT_STRING(array, Registers_names[reg_dst]); \
    INSERT_STRING_CONSTANT(array, ", "); \
    INSERT_STRING(array, Registers_names[reg_src]); \
    INSERT_STRING_CONSTANT(array, "\n");\
} \
\
static inline void name##_REG_MEM_NASM(Dynamic_array* array, int reg, int mem_reg, int32_t disp) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " "); \
    INSERT_STRING(array, Registers_names[reg]); \
    INSERT_STRING_CONSTANT(array, ", ["); \
    INSERT_STRING(array, Registers_names[mem_reg]); \
    INSERT_STRING_CONSTANT(array, " + "); \
    _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
    int symbols_written = sprintf(array->data + array->size, "%#x", disp);\
    array->size += symbols_written;\
    INSERT_STRING_CONSTANT(array, "]\n");\
} \
\
static inline void name##_MEM_REG_NASM(Dynamic_array* array, int mem_reg, int reg, int32_t disp) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " ["); \
    INSERT_STRING(array, Registers_names[mem_reg]); \
    INSERT_STRING_CONSTANT(array, " + "); \
    _Dynamic_array_resize_if_needed(array, Max_hex32_len + 3);\
    int symbols_written = sprintf(array->data + array->size, "%#x], ", disp);\
    array->size += symbols_written;\
    INSERT_STRING(array, Registers_names[reg]); \
    INSERT_STRING_CONSTANT(array, "\n"); \
} \
static inline void name##_REG_MEM_DISP0_NASM(Dynamic_array* array, int reg, int mem_reg) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " "); \
    INSERT_STRING(array, Registers_names[reg]); \
    INSERT_STRING_CONSTANT(array, ", ["); \
    INSERT_STRING(array, Registers_names[mem_reg]); \
    INSERT_STRING_CONSTANT(array, "]\n");\
} \
static inline void name##_MEM_REG_DISP0_NASM(Dynamic_array* array, int mem_reg, int reg) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " ["); \
    INSERT_STRING(array, Registers_names[mem_reg]); \
    INSERT_STRING_CONSTANT(array, "], "); \
    INSERT_STRING(array, Registers_names[reg]); \
    INSERT_STRING_CONSTANT(array, "\n"); \
} \
\
static inline void name##_REG_IMM_NASM(Dynamic_array* array, int reg, int32_t imm32) { \
    INSERT_STRING_CONSTANT(array, "\t" #name " "); \
    INSERT_STRING(array, Registers_names[reg]); \
    INSERT_STRING_CONSTANT(array, ", "); \
    _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
    int symbols_written = sprintf(array->data + array->size, "%#x", imm32);\
    array->size += symbols_written;\
    INSERT_STRING_CONSTANT(array, "\n");\
}
