#include "Main_modification_funcs.h"

bool Get_all_funcs_offset(char* file_buffer, size_t file_size,
                          Func_instr_params* func_params_array) {

    if(!Find_main_entry(file_buffer, file_size, func_params_array, Main_entry_index))
        return false;

    if(!Get_func_offset(file_buffer, file_size, func_params_array, Format_string_sf_index))
        return false;

    if(!Get_func_offset(file_buffer, file_size, func_params_array, Format_string_pf_index))
        return false;

    if(!Get_func_offset(file_buffer, file_size, func_params_array, Scanf_index))
        return false;

    if(!Get_func_offset(file_buffer, file_size, func_params_array, Printf_index))
        return false;

    return true;
}

bool Get_func_offset(char* file_buffer, size_t file_size, Func_instr_params* func_params, const int func_index) {

    size_t offset = 0;
    size_t index = 0;
    bool status = false;
    for(; index < file_size - func_params[func_index].bytes_count; index++) {

        if(!memcmp(file_buffer + index, func_params[func_index].bytes_before_func, func_params[func_index].bytes_count)) {

            status = true;
            break;
        }
    }

    if(!status) {

        DEBUG_PRINTF("ERROR: function was not found\n");
        return false;
    }

    else {

        memcpy(&offset, file_buffer + index + func_params[func_index].bytes_count + func_params[func_index].forward_offset,
               func_params[func_index].bytes_to_copy);     // (prev ver. comment)+ 1 = E8 - byte of instruction call rel32
        offset += index + func_params[func_index].bytes_count + func_params[func_index].forward_offset +
                  func_params[func_index].bytes_to_copy - func_params[Main_entry_index].rel_address;
    }

    func_params[func_index].rel_address = offset;

    return true;
}

bool Find_main_entry(char* file_buffer, size_t file_size, Func_instr_params* func_params, const int func_index) { //const uint8_t* bytes_to_find, int amount_of_bytes, int rel_entry_offset

    size_t offset = 0;
    size_t index = 0;
    bool status = false;
    for(; index < file_size - func_params[func_index].bytes_count; index++) {

        if(!memcmp(file_buffer + index, func_params[func_index].bytes_before_func, func_params[func_index].bytes_count)) {

            status = true;
            break;
        }
    }

    if(!status) {

        DEBUG_PRINTF("ERROR: function was not found\n");
        return false;
    }

    func_params[func_index].rel_address = index + func_params[func_index].forward_offset;

    return true;
}
