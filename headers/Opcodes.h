#ifndef OPCODES_X64
#define OPCODES_X64
enum Opcodes {

    PUSH_OP = 0x50,
    PUSH_M_DISP32_OP = 0xFF,

    POP_OP = 0x58,
    POP_M_OP = 0x8F,

    ADD_R_RM_OP = 0x03,
    ADD_RM_R_OP = 0x01,

    SUB_R_RM_OP = 0x2B,
    SUB_RM_R_OP = 0x29,

    CMP_R_RM_OP = 0x3B,
    CMP_RM_R_OP = 0x39,

    OR_R_RM_OP    = 0x0B,
    OR_RM_R_OP    = 0x09,

    ADC_R_RM_OP   = 0x13,
    ADC_RM_R_OP   = 0x11,

    SBB_R_RM_OP   = 0x1B,
    SBB_RM_R_OP   = 0x19,

    AND_R_RM_OP   = 0x23,
    AND_RM_R_OP   = 0x21,

    XOR_R_RM_OP   = 0x33,
    XOR_RM_R_OP   = 0x31,

    MOV_R_RM_OP   = 0x8B,
    MOV_RM_R_OP   = 0x89,

    TEST_RM_R_OP = 0x85,
    TEST_RM_IMM32_OP = 0xF7,

    NEG_REG_OP = 0xF7,
    MOV_R_IMM64_OP = 0xB8,

    LEA_R_RM_OP = 0x8D,

    INSRT_IMM32_OP = 0x81,
    MOVSD_OP = 0x10,
    ADDSD_OP = 0x58,
    SUBSD_OP = 0x5C,
    MULSD_OP = 0x59,
    DIVSD_OP = 0x5E,
    SQRTSD_OP = 0x51,
    COMISD_OP = 0X2F,

    XORPD_OP = 0x57,
    ANDPD_OP =0x54,

    COND_JMP_OP = 0x0F,
    CALL_REL32_OP = 0xE8,
    JMP_REL32_OP = 0xE9,
    JL_REL32_OP = 0x8C,
    JLE_REL32_OP = 0x8E,
    JG_REL32_OP = 0x8F,
    JGE_REL32_OP = 0x8D,
    JE_REL32_OP = 0x84,
    JNE_REL32_OP = 0x85,

    JB_REL8_OP = 0x72,
    JBE_REL8_OP = 0x76,
    JA_REL8_OP = 0x77,
    JAE_REL8_OP = 0x73,
    JE_REL8_OP = 0x74,
    JNE_REL8_OP = 0x75,

    NOP_OP = 0x90,
    RET_OP = 0xC3,
    LEAVE_OP = 0xC9
};

enum Special_bytes {

    MOD_R      = 0b11000000,
    MOD_DISP0  = 0b00000000,
    MOD_DISP8  = 0b01000000,
    MOD_DISP32 = 0b10000000,

    MOD_ADD = 0b000,
    MOD_OR  = 0b001,
    MOD_ADC = 0b010,
    MOD_SBB = 0b011,
    MOD_AND = 0b100,
    MOD_SUB = 0b101,
    MOD_XOR = 0b110,
    MOD_CMP = 0b111,

    MOD_MOV = 0b000,

    MOD_NEG = 0b011,

    MOD_PUSH = 0b110,
    MOD_POP = 0b000,

    PREFIX_DOUBLE1 = 0XF2,
    PREFIX_SSE2 = 0X66,
    ESCAPE_BYTE = 0x0F,

    // SIB for [rsp] case, index=none, scale=1
    SIB_RSP_ONLY = 0x24       // 0010 0100
};

enum Register_index {

    I_RAX = 0b000,
    I_RCX = 0b001,
    I_RDX = 0b010,
    I_RBX = 0b011,
    I_RSP = 0b100,
    I_RBP = 0b101,
    I_RSI = 0b110,
    I_RDI = 0b111,
    I_R8  = 0b1000,
    I_R9  = 0b1001,
    I_R10 = 0b1010,
    I_R11 = 0b1011,
    I_R12 = 0b1100,
    I_R13 = 0b1101,
    I_R14 = 0b1110,
    I_R15 = 0b1111
};

enum Xmm_Register_index {

    I_XMM0 = 0b000,
    I_XMM1 = 0b001,
    I_XMM2 = 0b010,
    I_XMM3 = 0b011,
    I_XMM4 = 0b100,
    I_XMM5 = 0b101,
    I_XMM6 = 0b110,
    I_XMM7 = 0b111,
    I_XMM8 = 0b1000,
    I_XMM9 = 0b1001,
    I_XMM10 = 0b1010,
    I_XMM11 = 0b1011,
    I_XMM12 = 0b1100,
    I_XMM13 = 0b1101,
    I_XMM14 = 0b1110,
    I_XMM15 = 0b1111
};

enum REX_bits {

    REX_W = 0b01001000,
    REX_R = 0b01000100,
    REX_X = 0b01000010,
    REX_B = 0b01000001
};

#endif
