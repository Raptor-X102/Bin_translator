#ifndef CREATE_MAIN
#define CREATE_MAIN

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "GetFileSize2.h"
#include "Debug_printf.h"
#include "Main_modification_funcs.h"

#define IN_OUT_MODULES_PATH "In_out_modules/In_out_modules.cpp"

#define NOP_SEPARATOR "\"\n\t\t\t\t\t\t  \""
#define NOP1 "nop\\n\\t"
#define NOP2   NOP1                 NOP1
#define NOP4   NOP2                 NOP2
#define NOP8   NOP4                 NOP4
#define NOP16  NOP8   NOP_SEPARATOR NOP8
#define NOP32  NOP16  NOP_SEPARATOR NOP16
#define NOP64  NOP32  NOP_SEPARATOR NOP32
#define NOP128 NOP64  NOP_SEPARATOR NOP64
#define NOP256 NOP128 NOP_SEPARATOR NOP128
static const char Asm_volatile_string1  [] = "\t__asm__ __volatile__ (\"" NOP1   "\");\n";
static const char Asm_volatile_string2  [] = "\t__asm__ __volatile__ (\"" NOP2   "\");\n";
static const char Asm_volatile_string4  [] = "\t__asm__ __volatile__ (\"" NOP4   "\");\n";
static const char Asm_volatile_string8  [] = "\t__asm__ __volatile__ (\"" NOP8   "\");\n";
static const char Asm_volatile_string16 [] = "\t__asm__ __volatile__ (\"" NOP16  "\");\n";
static const char Asm_volatile_string32 [] = "\t__asm__ __volatile__ (\"" NOP32  "\");\n";
static const char Asm_volatile_string64 [] = "\t__asm__ __volatile__ (\"" NOP64  "\");\n";
static const char Asm_volatile_string128[] = "\t__asm__ __volatile__ (\"" NOP128 "\");\n";
static const char Asm_volatile_string256[] = "\t__asm__ __volatile__ (\"" NOP256 "\");\n";
static const int Base_asm_volatile_strings_amount = 9;
static const int Instructions_bytes_limit = 16; // max 2^16 bytes for code instructions
static const char* Asm_string_array[Base_asm_volatile_strings_amount] =
                                        {Asm_volatile_string1,
                                         Asm_volatile_string2,
                                         Asm_volatile_string4,
                                         Asm_volatile_string8,
                                         Asm_volatile_string16,
                                         Asm_volatile_string32,
                                         Asm_volatile_string64,
                                         Asm_volatile_string128,
                                         Asm_volatile_string256};
static const char Ending_symbols[] = "}\n";
static const uint64_t Asm_strings_lens[] = {   sizeof(Asm_volatile_string1  ) - 1,
                                        sizeof(Asm_volatile_string2  ) - 1,
                                        sizeof(Asm_volatile_string4  ) - 1,
                                        sizeof(Asm_volatile_string8  ) - 1,
                                        sizeof(Asm_volatile_string16 ) - 1,
                                        sizeof(Asm_volatile_string32 ) - 1,
                                        sizeof(Asm_volatile_string64 ) - 1,
                                        sizeof(Asm_volatile_string128) - 1,
                                        sizeof(Asm_volatile_string256) - 1,
                                        (sizeof(Asm_volatile_string256) - 1)*2,
                                        (sizeof(Asm_volatile_string256) - 1)*4,
                                        (sizeof(Asm_volatile_string256) - 1)*8,
                                        (sizeof(Asm_volatile_string256) - 1)*16,
                                        (sizeof(Asm_volatile_string256) - 1)*32,
                                        (sizeof(Asm_volatile_string256) - 1)*64,
                                        (sizeof(Asm_volatile_string256) - 1)*128};

Func_instr_params* Create_main(const char* filename1, const char* filename2,
                               char** target_filename_ptr, char** buffer,
                               size_t* buffer_size_ptr, size_t nop_amount);
char* Compile_file(const char* filename);
const int Target_name_size = 128;
const int Filename_buffer_add = 5; // .fmt + \0 (fmt = cpp / exe)
#endif
