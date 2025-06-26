#ifndef BACKEND_FUNCS_X64
#define BACKEND_FUNCS_X64

#include "Lang_funcs.h"
#include "Create_main.h"
#include "Dynamic_array_funcs.h"
#include "Memcpy_upgrade.h"

const size_t D_array_code_init_size = 2048;
const size_t D_array_call_init_size = 5;

const int32_t No_jmp_rel32 = 0;    // mostly used just to pre-fill instructions with rel address
const int8_t No_jmp_rel8 = 0;      //
const int8_t Skip_5_bytes = 5;

const int Value_size = 8;          // size of variable type (double in my language)
const int32_t Rel32_code_size = 4; // size in bytes encoding sequence of relative displacement 32bits

const char Check_alignment_even[] = "Check_alignment_even";
const char Check_alignment_odd[] = "Check_alignment_odd";
const int Shadow_space = 32;       // windows stuff
const int Code_alignment = 16;

const int Main_prologue_length = 0x68; // WARNING! if ypu add more std funcs, it must be changed
const int Main_epilogue_length = 11;

const int64_t Double_sign_mask = 0x7FFFFFFFFFFFFFFF;    // for abs function

const int32_t Format_string_sf_addr_index = 0xFA11;
const int32_t Format_string_pf_addr_index = 0xCA11;
const int32_t Scanf_func_index = 0x5CA9F;
const int32_t Printf_func_index = 0xFAC75;

const int Direct_option = 0x1F; // IF               for Compile_condition_x64
const int Reverse_option = 0x9071F; // NOT_IF ) <-- was used before I changed logic of jmps
                                    //              due to absence of unsigned jxx (jb, jae etc.) rel32, there are only rel8 ((((

#include "Opcodes.h"        // all opcodes used to encode

extern const char* Registers_names  [];     // definition in "Backend_funcs_x64.cpp"
extern const char* X_Registers_names[];     //

#ifndef NO_INSTRUCTIONS_DEBUG
#define INSTR_DEBUG_PRINTF(...) fprintf(stderr, __VA_ARGS__)
#define INSTR_DEBUG(...) __VA_ARGS__
#else
#define INSTR_DEBUG_PRINTF(...)
#define INSTR_DEBUG(...)
#endif

#define INSTR_DEBUG_OUT_LOOP(array, bytes_count) \
        INSTR_DEBUG(for(int d_i = 0; d_i < bytes_count; d_i++) {)\
        INSTR_DEBUG(uint8_t byte = array->data[array->size - bytes_count + d_i];)\
        INSTR_DEBUG_PRINTF("%02x ", byte);\
        INSTR_DEBUG(})\
        INSTR_DEBUG_PRINTF("\n");

#define INSERT_CHECK_FUNCS(check_alignment, func_list) \
        Func_data check_alignment##_func_data = {};\
        Variable_data* check_alignment##var_data = (Variable_data*) calloc(1, sizeof(Variable_data));\
        check_alignment##var_data->var_name = Check_alignment_even;\
        check_alignment##var_data->var_len = sizeof(Check_alignment_even) - 1;\
        check_alignment##_func_data.function = check_alignment##var_data;\
        func_list->func_data[func_list->free_element] = check_alignment##_func_data;\
        func_list->free_element++;

/******************USED DEFINES***************************************** */
#define PUSH_REG(array, REG) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, PUSH_OP | REG);\
        INSTR_DEBUG_PRINTF("PUSH " #REG " = ");\
        INSTR_DEBUG_OUT_LOOP(array, 2)

#define PUSH_RBP_DISP32(array, DISP32)  \
        INSERT_BYTE(array, PUSH_M_DISP32_OP); \
        INSERT_BYTE(array, MOD_DISP32 | (MOD_PUSH << 3) | (I_RBP & 7));\
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("PUSH [RBP + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 6)

#define NEG_REG(array, REG) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, NEG_REG_OP);\
        INSERT_BYTE(array, MOD_R | (MOD_NEG << 3) | (REG & 7));\
        INSTR_DEBUG_PRINTF("NEG " #REG " = ");\
        INSTR_DEBUG_OUT_LOOP(array, 3)

#define MOV_REG_IMM64(array, REG_DST, IMM64) \
        INSERT_BYTE(array, REX_W | ((REG_DST >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, MOV_R_IMM64_OP | (REG_DST & 7)); \
        INSERT_QWORD(array, IMM64);\
        INSTR_DEBUG_PRINTF("MOV " #REG_DST ", %lf (double) / %lld (int64_t) = ", IMM64, IMM64);\
        INSTR_DEBUG_OUT_LOOP(array, 10)

#define LEA_REG_RIP_REL32(array, REG, DISP32) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0)); \
        INSERT_BYTE(array, LEA_R_RM_OP); \
        INSERT_BYTE(array, ((REG & 7) << 3) | 0b101); \
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("LEA " #REG ", [RIP + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 7)

#define LEA_REG_MEM(array, REG, BASE_REG, DISP32) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0) | ((BASE_REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, LEA_R_RM_OP); \
        INSERT_BYTE(array, MOD_DISP32 | ((REG & 7) << 3) | (BASE_REG & 7)); \
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("LEA " #REG ", [" #BASE_REG " + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 7)

#define MOVSD_XREG_MEM_DISP0(array, REG, BASE_REG) \
        INSERT_BYTE(array, PREFIX_DOUBLE1); \
        if (((REG) >= 8) || ((BASE_REG) >= 8)) {\
            INSERT_BYTE(array, 0 | ((REG >= 8) ? REX_R : 0) | ((BASE_REG >= 8) ? REX_B : 0)); }\
        INSERT_BYTE(array, ESCAPE_BYTE); \
        INSERT_BYTE(array, MOVSD_OP); \
        INSERT_BYTE(array, MOD_DISP0 | \
            (((REG) & 7) << 3) | \
            ((BASE_REG) & 7)); \
        if (((BASE_REG) & 7) == I_RSP) \
        INSERT_BYTE(array, SIB_RSP_ONLY); \
        INSTR_DEBUG_PRINTF("MOVSD " #REG ", [" #BASE_REG "] = "); \
        INSTR_DEBUG_OUT_LOOP(array, 5)

#define MOVSD_MEM_XREG(array, BASE_REG, REG, DISP32) \
        INSERT_BYTE(array, PREFIX_DOUBLE1);  \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0) | ((BASE_REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, ESCAPE_BYTE);  \
        INSERT_BYTE(array, MOVSD_OP + 1);   /* 0x11 movsd m64, xmm*/ \
        INSERT_BYTE(array, MOD_DISP32 | ((REG & 7) << 3) | (BASE_REG & 7));\
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("MOVSD [" #BASE_REG " + %x], " #REG " = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 9)

#define MOVSD_MEM_XREG_DISP0(array, BASE_REG, REG) \
        INSERT_BYTE(array, PREFIX_DOUBLE1); \
        if (((REG) >= 8) || ((BASE_REG) >= 8)) {\
            INSERT_BYTE(array, 0 | ((REG >= 8) ? REX_R : 0) | ((BASE_REG >= 8) ? REX_B : 0)); }\
        INSERT_BYTE(array, ESCAPE_BYTE); \
        INSERT_BYTE(array, MOVSD_OP + 1); \
        INSERT_BYTE(array, MOD_DISP0 | (((REG) & 7) << 3) | ((BASE_REG) & 7)); \
        if (((BASE_REG) & 7) == I_RSP) \
            INSERT_BYTE(array, SIB_RSP_ONLY); \
        INSTR_DEBUG_PRINTF("MOVSD [" #BASE_REG "], " #REG " = "); \
        INSTR_DEBUG_OUT_LOOP(array, 5)

#define RET(array) \
        INSERT_BYTE(array, RET_OP); \
        INSTR_DEBUG_PRINTF("RET = "); \
        INSTR_DEBUG_OUT_LOOP(array, 1)

#define LEAVE(array) \
        INSERT_BYTE(array, LEAVE_OP); \
        INSTR_DEBUG_PRINTF("LEAVE = "); \
        INSTR_DEBUG_OUT_LOOP(array, 1)

#include "Arithm_instr_codegen.h"
DEFINE_INSTRUCTION_FUNCS(ADD)
DEFINE_INSTRUCTION_FUNCS(OR)
DEFINE_INSTRUCTION_FUNCS(ADC)
DEFINE_INSTRUCTION_FUNCS(SBB)
DEFINE_INSTRUCTION_FUNCS(AND)
DEFINE_INSTRUCTION_FUNCS(SUB)
DEFINE_INSTRUCTION_FUNCS(XOR)
DEFINE_INSTRUCTION_FUNCS(CMP)
DEFINE_INSTRUCTION_FUNCS(MOV)
#undef DEFINE_INSTRUCTION_FUNCS

#include "Instrsd_codegen1.h"
DEFINE_INSTRUCTION_FUNCS(ADDSD)
DEFINE_INSTRUCTION_FUNCS(SUBSD)
DEFINE_INSTRUCTION_FUNCS(MULSD)
DEFINE_INSTRUCTION_FUNCS(DIVSD)
DEFINE_INSTRUCTION_FUNCS(SQRTSD)
#undef DEFINE_INSTRUCTION_FUNCS

#include "Instrsd_codegen2.h"
DEFINE_INSTRUCTION_FUNCS(COMISD)
DEFINE_INSTRUCTION_FUNCS(XORPD)
DEFINE_INSTRUCTION_FUNCS(ANDPD)
#undef DEFINE_INSTRUCTION_FUNCS

#include "Jxx_instr_codegen.h"
JXX_SIGNED(JL)
JXX_SIGNED(JLE)
JXX_SIGNED(JG)
JXX_SIGNED(JGE)
JXX_SIGNED(JE)
JXX_SIGNED(JNE)

JXX_UNSIGNED(JB)
JXX_UNSIGNED(JBE)
JXX_UNSIGNED(JA)
JXX_UNSIGNED(JAE)
JXX_UNSIGNED(JE)
JXX_UNSIGNED(JNE)
#undef DEFINE_INSTRUCTION_FUNCS

#include "Call_jmp_instr_codegen.h"
DEFINE_INSTRUCTION_FUNCS(CALL)
DEFINE_INSTRUCTION_FUNCS(JMP)
#undef DEFINE_INSTRUCTION_FUNCS
/*********************************************************************** */

/**************NOT USED DEFINES***************************************** */
#define POP_REG(array, REG) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, POP_OP | (REG & 7));\
        INSTR_DEBUG_PRINTF("POP " #REG " = ");\
        INSTR_DEBUG_OUT_LOOP(array, 2)

#define POP_MEM_DISP32(array, BASE_REG, DISP32) \
        INSERT_BYTE(array, REX_W | ((BASE_REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, POP_M_OP); \
        INSERT_BYTE(array, MOD_DISP32 | (MOD_POP << 3) | (BASE_REG & 7)); \
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("POP [" #BASE_REG " + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 2)

#define TEST_REG_IMM32(array, REG, IMM32) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, TEST_RM_IMM32_OP);\
        INSERT_BYTE(array, MOD_R | (REG & 7));\
        INSERT_DWORD(array, IMM32);

#define MOV_REG_RIP_REL(array, REG, DISP32) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0)); \
        INSERT_BYTE(array, MOV_R_RM_OP); \
        INSERT_BYTE(array, ((REG & 7) << 3) | 0b101); \
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("MOV " #REG ", [RIP + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 7)

#define MOV_RIP_REL_REG(array, REG, DISP32) \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0)); \
        INSERT_BYTE(array, MOV_RM_R_OP); \
        INSERT_BYTE(array, ((REG & 7) << 3) | 0b101); \
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("MOV [RIP + %x], " #REG " = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 7)

#define MOVSD_XREG_XREG(array, REG_DST, REG_SRC) \
        INSERT_BYTE(array, PREFIX_DOUBLE1);  \
        INSERT_BYTE(array, REX_W | ((REG_DST >= 8) ? REX_R : 0) | ((REG_SRC >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, ESCAPE_BYTE);     \
        INSERT_BYTE(array, MOVSD_OP + 1);   /* 0x11 (MOVSD xmm/mem, xmm)*/ \
        INSERT_BYTE(array, MOD_R | ((REG_DST & 7) << 3) | (REG_SRC & 7));\
        INSTR_DEBUG_PRINTF("MOVSD " #REG_DST ", " #REG_SRC " = ");\
        INSTR_DEBUG_OUT_LOOP(array, 5)

#define MOVSD_XREG_MEM(array, REG, BASE_REG, DISP32) \
        INSERT_BYTE(array, PREFIX_DOUBLE1); \
        INSERT_BYTE(array, REX_W | ((REG >= 8) ? REX_R : 0) | ((BASE_REG >= 8) ? REX_B : 0)); \
        INSERT_BYTE(array, ESCAPE_BYTE);    \
        INSERT_BYTE(array, MOVSD_OP);      \
        INSERT_BYTE(array, MOD_DISP32 | ((REG & 7) << 3) | (BASE_REG & 7));\
        INSERT_DWORD(array, DISP32);\
        INSTR_DEBUG_PRINTF("MOVSD " #REG ", [" #BASE_REG " + %x] = ", DISP32);\
        INSTR_DEBUG_OUT_LOOP(array, 9)
/*********************************************************************** */

bool Compile_x64(const char* input_filename,
                 const char* output_filename,
                 Node* root, Func_data_list* func_list);
void Fill_jmp_call_addresses(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                             Func_data_list* func_list, Func_instr_params* std_funcs);
void Check_alignment_prologue_x64(Dynamic_array* d_array_code, Func_data_list* func_list,
                              int64_t check_align_even_index, int64_t check_align_odd_index);
bool Compile_user_function_def_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                                   Node* root, Func_data_list* func_list);
bool Compile_operation_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                           Func_data_list* func_list, int64_t index);
bool Compile_while_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                       Func_data_list* func_list, int64_t index);
bool Compile_if_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                    Func_data_list* func_list, int64_t index);
uint32_t Compile_condition_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                               Node* root, Func_data_list* func_list, int64_t index, int option);
bool Compile_return_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                        int64_t index);
bool Compile_assignment_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                            int64_t index);
bool Compile_operator_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                          int64_t index);
bool Compile_user_function_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                               Node* root, Func_data_list* func_list, int64_t index);
bool Compile_Out_function_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                              Node* root, Func_data_list* func_list, int64_t index);
bool Compile_In_function_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                              Node* root, Func_data_list* func_list, int64_t index);
bool Compile_push_parameters_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                                 int64_t index, int* parameters_pushed);

#endif
