#ifndef MAIN_MOD_FUNCS
#define MAIN_MOD_FUNCS

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "GetFileSize2.h"
#include "Debug_printf.h"

struct Funcs_Offset {

    size_t foramt_string_sf_offset;
    size_t foramt_string_pf_offset;
    size_t printf_offset;
    size_t scanf_offset;
};

struct Func_instr_params {

    const uint8_t* bytes_before_func;
    int bytes_count;
    int forward_offset;
    int bytes_to_copy;
    size_t rel_address;
};

bool Get_all_funcs_offset(char* file_buffer, size_t file_size,
                          Func_instr_params* func_params_array);

bool Get_func_offset(char* file_buffer, size_t file_size, Func_instr_params* func_params, const int func_index);
bool Find_main_entry(char* file_buffer, size_t file_size, Func_instr_params* func_params, const int func_index);
enum Func_arrays_index {

    Main_entry_index = 0x0,
    Format_string_sf_index = 0x1,
    Format_string_pf_index = 0x2,
    Printf_index = 0x3,
    Scanf_index = 0x4
};

const int Funcs_total = 5;

const int Bytes_to_copy_adr_rel32 = 4;

//const uint8_t Bytes_before_fs_sf[] = { 0xc1, 0xb8, 0x98, 0x1d};
//const uint8_t Bytes_before_fs_sf[] = { 0xc1, 0xb8, 0x98, 0x1d};
const int Forward_offset_fs_sf = 10;
const int Forward_offset_fs_pf = 28;
const int Main_entry_offset_format_str = -30;
/*************Math functions************************************************/




/*************In-Out functions************************************************/
const uint8_t Bytes_before_pf[] = { 0x35, 0x70, 0x40, 0x02};
const int Forward_offset_pf = 36;

const uint8_t Bytes_before_sf[] = { 0xc1, 0xb8, 0x98, 0x1d};
const int Forward_offset_sf = 18;

#define INITIALIZE_FUNCS_PARAMS(funcs_params) funcs_params[Main_entry_index].bytes_before_func   = Bytes_before_sf;\
                                              funcs_params[Main_entry_index].bytes_count         = sizeof(Bytes_before_sf) / sizeof(Bytes_before_sf[0]);\
                                              funcs_params[Main_entry_index].forward_offset      = Main_entry_offset_format_str;\
                                              funcs_params[Main_entry_index].bytes_to_copy       = Bytes_to_copy_adr_rel32;\
                                              funcs_params[Format_string_sf_index].bytes_before_func   = Bytes_before_sf;\
                                              funcs_params[Format_string_sf_index].bytes_count         = sizeof(Bytes_before_sf) / sizeof(Bytes_before_sf[0]);\
                                              funcs_params[Format_string_sf_index].forward_offset      = Forward_offset_fs_sf;\
                                              funcs_params[Format_string_sf_index].bytes_to_copy       = Bytes_to_copy_adr_rel32;\
                                              funcs_params[Format_string_pf_index].bytes_before_func   = Bytes_before_pf;\
                                              funcs_params[Format_string_pf_index].bytes_count         = sizeof(Bytes_before_pf) / sizeof(Bytes_before_pf[0]);\
                                              funcs_params[Format_string_pf_index].forward_offset      = Forward_offset_fs_pf;\
                                              funcs_params[Format_string_pf_index].bytes_to_copy       = Bytes_to_copy_adr_rel32;\
                                              funcs_params[Printf_index].bytes_before_func   = Bytes_before_pf;\
                                              funcs_params[Printf_index].bytes_count         = sizeof(Bytes_before_pf) / sizeof(Bytes_before_pf[0]);\
                                              funcs_params[Printf_index].forward_offset      = Forward_offset_pf;\
                                              funcs_params[Printf_index].bytes_to_copy       = Bytes_to_copy_adr_rel32;\
                                              funcs_params[Scanf_index ].bytes_before_func   = Bytes_before_sf;\
                                              funcs_params[Scanf_index ].bytes_count         = sizeof(Bytes_before_sf) / sizeof(Bytes_before_sf[0]);\
                                              funcs_params[Scanf_index ].forward_offset      = Forward_offset_sf;\
                                              funcs_params[Scanf_index ].bytes_to_copy       = Bytes_to_copy_adr_rel32;

#endif
