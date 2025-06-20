#ifndef BACKEND_FUNCS_NAMS
#define BACKEND_FUNCS_NAMS

#include "Lang_funcs.h"
#include "Create_main.h"
#include "Dynamic_array_funcs.h"
#include "Memcpy_upgrade.h"

extern const size_t D_array_code_init_size;
extern const int Value_size;
extern const char Check_alignment_even[];
extern const char Check_alignment_odd[];        // all of these constants are defined in "Backend_funcs_x64.h"
extern const int Direct_option;
extern const int Reverse_option;
extern const int64_t Double_sign_mask;

#include "Opcodes.h"

extern const char* Registers_names  [];
extern const char* X_Registers_names[];

/*****************************BEGINNING OF NASM DEFINES********************** */
const char Prologue_data_nasm[] = "section .data\n\t"
                                      "fmt_out db \"%.3lf\", 10, 0\n\t"
                                      "fmt_in  db \"%lf\", 0\n\n";
const char Prologue_text_nasm[] = "section .text\n\t"
                                      "global main\n\t"
                                      "extern printf\n\t"
                                      "extern scanf\n";

const int Max_hex32_len = 10;       // 0x FF FF FF FF
                                    //  2  2  2  2  2 = 10
const int Max_hex64_len = 18;       // 0x FF FF FF FF FF FF FF FF
                                    //  2  2  2  2  2  2  2  2  2 = 18

#define INSERT_STRING_CONSTANT(array, text) Dynamic_array_push_back(array, text, sizeof(text) - 1)
#define INSERT_STRING(array, text) Dynamic_array_push_back(array, text, strlen(text))
#define INSERT_TEXT(array, text, length) Dynamic_array_push_back(array, text, length)

#define PUSH_REG_NASM(array, REG) \
        INSERT_STRING_CONSTANT(array, "\tPUSH "); \
        INSERT_STRING(array, Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, " \n");

#define PUSH_RBP_DISP32_NASM(array, DISP32)  \
        INSERT_STRING_CONSTANT(array, "\tPUSH QWORD [RBP + "); \
        _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
        symbols_written = sprintf(array->data + array->size, "%#x", DISP32);\
        array->size += symbols_written;\
        INSERT_STRING_CONSTANT(array, "]\n");

#define NEG_REG_NASM(array, REG) \
		INSERT_STRING_CONSTANT(array, "\tNEG "); \
		INSERT_STRING(array, Registers_names[REG]); \
		INSERT_STRING_CONSTANT(array, "\n");

#define INSERT_LABEL(array, loc_index) \
		INSERT_STRING_CONSTANT(array, ".L"); \
		_Dynamic_array_resize_if_needed(array, Max_hex32_len);\
		symbols_written = sprintf(array->data + array->size, "%x", loc_index);\
		array->size += symbols_written;\
		INSERT_STRING_CONSTANT(array, ":\n");

#define MOV_REG_IMM64_NASM(array, REG_DST, IMM64) \
        INSERT_STRING_CONSTANT(array, "\tMOV "); \
        INSERT_STRING(array, Registers_names[REG_DST]); \
        INSERT_STRING_CONSTANT(array, ", "); \
        _Dynamic_array_resize_if_needed(array, Max_hex64_len);\
        symbols_written = sprintf(array->data + array->size, "%#llx", IMM64);\
        array->size += symbols_written;\
        INSERT_STRING_CONSTANT(array, "\n");

#define LEA_REG_RIP_REL_NASM(array, REG, label) \
        INSERT_STRING_CONSTANT(array, "\tLEA "); \
        INSERT_STRING(array, Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, ", [REL " label "]\n");

#define LEA_REG_MEM_NASM(array, REG, BASE_REG, DISP32) \
        INSERT_STRING_CONSTANT(array, "\tLEA "); \
        INSERT_STRING(array, Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, ", ["); \
        INSERT_STRING(array, Registers_names[BASE_REG]); \
        INSERT_STRING_CONSTANT(array, " + "); \
        _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
        symbols_written = sprintf(array->data + array->size, "%#x", DISP32);\
        array->size += symbols_written;\
        INSERT_STRING_CONSTANT(array, "]\n");

#define MOVSD_MEM_XREG_NASM(array, BASE_REG, REG, DISP32) \
        INSERT_STRING_CONSTANT(array, "\tMOVSD ["); \
        INSERT_STRING(array, Registers_names[BASE_REG]); \
        INSERT_STRING_CONSTANT(array, " + "); \
        _Dynamic_array_resize_if_needed(array, Max_hex32_len + 3);\
        symbols_written = sprintf(array->data + array->size, "%#x], ", DISP32);\
        array->size += symbols_written;\
        INSERT_STRING(array, X_Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, "\n");

#define MOVSD_XREG_MEM_DISP0_NASM(array, REG, BASE_REG) \
        INSERT_STRING_CONSTANT(array, "\tMOVSD "); \
        INSERT_STRING(array, X_Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, ", ["); \
        INSERT_STRING(array, Registers_names[BASE_REG]); \
        INSERT_STRING_CONSTANT(array, "]\n");

#define MOVSD_MEM_XREG_DISP0_NASM(array, BASE_REG, REG) \
        INSERT_STRING_CONSTANT(array, "\tMOVSD ["); \
        INSERT_STRING(array, Registers_names[BASE_REG]); \
        INSERT_STRING_CONSTANT(array, "], "); \
        INSERT_STRING(array, X_Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, "\n");

#define RET_NASM(array) INSERT_STRING_CONSTANT(array, "\tRET\n");
#define LEAVE_NASM(array) INSERT_STRING_CONSTANT(array, "\tLEAVE\n");

#include "Arithm_nasm_codegen.h"
DEFINE_NASM_FUNCS(ADD)
DEFINE_NASM_FUNCS(OR)
DEFINE_NASM_FUNCS(ADC)
DEFINE_NASM_FUNCS(SBB)
DEFINE_NASM_FUNCS(AND)
DEFINE_NASM_FUNCS(SUB)
DEFINE_NASM_FUNCS(XOR)
DEFINE_NASM_FUNCS(CMP)
DEFINE_NASM_FUNCS(MOV)
#undef DEFINE_NASM_FUNCS

#include "Jxx_nasm_codegen.h"
JXX_NASM(JL)
JXX_NASM(JLE)
JXX_NASM(JG)
JXX_NASM(JGE)

JXX_NASM(JB)
JXX_NASM(JBE)
JXX_NASM(JA)
JXX_NASM(JAE)

JXX_NASM(JE)
JXX_NASM(JNE)
#undef JXX_NASM

#include "Call_jmp_nasm_codegen.h"
DEFINE_NASM_FUNCS(CALL)
DEFINE_NASM_FUNCS(JMP)
#undef DEFINE_NASM_FUNCS

#include "Instrsd_nasm_codegen1.h"
DEFINE_NASM_FUNCS(ADDSD)
DEFINE_NASM_FUNCS(SUBSD)
DEFINE_NASM_FUNCS(MULSD)
DEFINE_NASM_FUNCS(DIVSD)
DEFINE_NASM_FUNCS(SQRTSD)
DEFINE_NASM_FUNCS(COMISD)
DEFINE_NASM_FUNCS(XORPD)
DEFINE_NASM_FUNCS(ANDPD)
#undef DEFINE_NASM_FUNCS
/*****************************ENDING OF NASM DEFINES********************** */

/*****************************NOT USED************************************ */

#define POP_REG_NASM(array, REG) \
        INSERT_STRING_CONSTANT(array, "\tPOP "); \
        INSERT_STRING(array, Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, " \n");

#define POP_MEM_DISP32_NASM(array, BASE_REG, DISP32) \
		INSERT_STRING_CONSTANT(array, "\tPOP ["); \
		INSERT_STRING(array, Registers_names[BASE_REG]); \
		INSERT_STRING_CONSTANT(array, " + "); \
		_Dynamic_array_resize_if_needed(array, Max_hex32_len);\
		symbols_written = sprintf(array->data + array->size, "%#x", DISP32);\
		array->size += symbols_written;\
		INSERT_STRING_CONSTANT(array, "]\n");

#define MOVSD_XREG_XREG_NASM(array, REG_DST, REG_SRC) \
        INSERT_STRING_CONSTANT(array, "\tMOVSD "); \
        INSERT_STRING(array, X_Registers_names[REG_DST]); \
        INSERT_STRING_CONSTANT(array, ", "); \
        INSERT_STRING(array, X_Registers_names[REG_SRC]); \
        INSERT_STRING_CONSTANT(array, "\n");

#define MOVSD_XREG_MEM_NASM(array, REG, BASE_REG, DISP32) \
        INSERT_STRING_CONSTANT(array, "\tMOVSD "); \
        INSERT_STRING(array, X_Registers_names[REG]); \
        INSERT_STRING_CONSTANT(array, ", ["); \
        INSERT_STRING(array, Registers_names[BASE_REG]); \
        INSERT_STRING_CONSTANT(array, " + "); \
        _Dynamic_array_resize_if_needed(array, Max_hex32_len);\
        symbols_written = sprintf(array->data + array->size, "%#x", disp);\
        array->size += symbols_written;\
        INSERT_STRING_CONSTANT(array, "]\n");
/************************************************************************* */

bool Compile_nasm(const char* output_file_name_nasm,
                 Node* root, Func_data_list* func_list);
void Check_alignment_prologue_nasm(Dynamic_array* d_array_code, Func_data_list* func_list);
bool Compile_user_function_def_nasm(Dynamic_array* d_array_code,Node* root, Func_data_list* func_list,
                                    int* local_labels_count);
bool Compile_operation_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                            int64_t index, int* local_labels_count);
bool Compile_while_nasm(Dynamic_array* d_array_code, Node* root,
                       Func_data_list* func_list, int64_t index, int* local_labels_count);
bool Compile_if_nasm(Dynamic_array* d_array_code, Node* root,
                    Func_data_list* func_list, int64_t index, int* local_labels_count);
bool Compile_condition_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list, int64_t index,
                            int option, int* local_labels_count);
bool Compile_return_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                        int64_t index);
bool Compile_assignment_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                            int64_t index);
bool Compile_operator_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                          int64_t index);
bool Compile_user_function_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list, int64_t index);
bool Compile_push_parameters_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                                 int64_t index, int* parameters_pushed);

#endif
