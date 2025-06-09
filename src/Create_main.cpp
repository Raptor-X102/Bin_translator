#include "Create_main.h"

Func_instr_params* Create_main(const char* filename1, const char* filename2,
                               char** target_filename_ptr, char** buffer,
                               size_t* buffer_size_ptr, size_t nop_amount) {

    Func_instr_params* funcs_params = (Func_instr_params*) calloc(Funcs_total, sizeof(Func_instr_params));
    size_t file_size = get_file_size(filename1);

    FILE* input_file = fopen(filename1, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    //int asm_volatile_strlen = sizeof(Asm_volatile_string1)-1;
    int ending_symbols_strlen = sizeof(Ending_symbols)-1;
    size_t buffer_size = ending_symbols_strlen + file_size;
    for(int i = 0; i < Base_asm_volatile_strings_amount; i++)
        buffer_size += ((nop_amount >> i) & 1) * Asm_strings_lens[i];

    for(int i = Base_asm_volatile_strings_amount; i < Instructions_bytes_limit; i++)
        buffer_size += ((nop_amount >> i) & 1) *
                        Asm_strings_lens[Base_asm_volatile_strings_amount-1] *
                        (2 << (i-Base_asm_volatile_strings_amount));

    char* file_buffer = (char*) calloc(buffer_size, sizeof(char));
    if(!file_buffer)  {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    fread(file_buffer, sizeof(char), file_size, input_file);

    file_buffer += file_size;
    fclose(input_file);

    char* filename_buffer = (char*) calloc(strlen(filename2) + Filename_buffer_add, sizeof(char));
    char* dot_ptr = strchr(filename2, '.');
    if(!dot_ptr)
        sprintf(filename_buffer, "%s.cpp");

    else {

        int file_len = (size_t) (dot_ptr - filename2);
        memcpy(filename_buffer, filename2, file_len);
        memcpy(filename_buffer + file_len, ".cpp", sizeof(".cpp"));
    }

    FILE* output_file = fopen(filename_buffer, "wb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    size_t written_bytes = 0;
    for(int i = 0; i < Base_asm_volatile_strings_amount; i++) {

        if((nop_amount & (1 << i))) {

            memcpy(file_buffer + written_bytes,
                   Asm_string_array[i],
                   Asm_strings_lens[i]);

            written_bytes += Asm_strings_lens[i];
        }
    }

    for(int i = Base_asm_volatile_strings_amount; i < Instructions_bytes_limit; i++) {

        if((nop_amount & (1 << i))) {

            for(int j = 0; j < (2 << (i-Base_asm_volatile_strings_amount)); j++) {

                memcpy(file_buffer + written_bytes,
                       Asm_string_array[Base_asm_volatile_strings_amount-1],
                       Asm_strings_lens[Base_asm_volatile_strings_amount-1]);

                written_bytes += Asm_strings_lens[Base_asm_volatile_strings_amount-1];
            }
        }
    }

    memcpy(file_buffer + written_bytes,
           Ending_symbols, ending_symbols_strlen);

    file_buffer -= file_size;
    fwrite(file_buffer, sizeof(char), buffer_size, output_file);
    fclose(output_file);
    free(file_buffer);

    char* target = Compile_file(filename_buffer);
    *target_filename_ptr = target;
    DEBUG_PRINTF("target = %s\n", target);
    file_size = get_file_size(target);
    *buffer_size_ptr = file_size;
    file_buffer = (char*) calloc(file_size, sizeof(char));
    if(!file_buffer)  {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    FILE* target_file = fopen(target, "rb");
    if(!target_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    fread(file_buffer, sizeof(char), file_size, target_file);
    if (feof(target_file)) {
        DEBUG_PRINTF("EOF достигнут\n");
    }
    if (ferror(target_file)) {
        DEBUG_PRINTF("Ошибка чтения файла\n");
    }
    *buffer = file_buffer;
    INITIALIZE_FUNCS_PARAMS(funcs_params)

    DEBUG_PRINTF("Get_all_funcs_offset STARTED\n");
    Get_all_funcs_offset(file_buffer, file_size, funcs_params);
    DEBUG_PRINTF("Get_all_funcs_offset ENDED\n");
    //printf("printf_offset = %zd\n", funcs_offset.printf_offset);
    //printf("scanf_offset = %zd\n", funcs_offset.scanf_offset);
    int fclose_result = fclose(target_file);
    if (fclose_result != 0) {
        DEBUG_PRINTF("Ошибка при закрытии файла: %d\n", fclose_result);
        perror("fclose error");
    }
    DEBUG_PRINTF("FILE CLOSED\n");
//**********************Testing ******************/
/*
    output_file = fopen(target, "wb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return funcs_offset;
    }
    int bytes_count = sizeof(Bytes_before_pf) / sizeof(Bytes_before_pf[0]);
    size_t new_printf_offset = funcs_offset.printf_offset - bytes_count - 5;
    size_t index = 0;
    bool status = false;
    for(; index < file_size - bytes_count; index++) {

        if(!memcmp(file_buffer + index, Bytes_before_pf, bytes_count)) {

            status = true;
            break;
        }
    }

    if(!status) {

        DEBUG_PRINTF("ERROR: function was not found\n");
    }

    else {

        new_printf_offset -= index;
        uint8_t call_rel32_byte = 0xe8;
        memcpy(file_buffer + index + bytes_count + 5, Bytes_before_pf, bytes_count);
        memcpy(file_buffer + index + 2*bytes_count + 5, &call_rel32_byte, sizeof(uint8_t));
        memcpy(file_buffer + index + 2*bytes_count + 5 + 1, &new_printf_offset, sizeof(int));
    }

    fwrite(file_buffer, sizeof(char), file_size, output_file);

//************************************* */
    //free(file_buffer);
    return funcs_params;
}

char* Compile_file(const char* filename) {

    char* target = (char*) calloc(strlen(filename) + 10, sizeof(char));
    char command[512];

    snprintf(target, Target_name_size, "%.*s.exe",
            (int)(strrchr(filename, '.') - filename), filename);

    snprintf(command, sizeof(command), "make create_main FILE=%s TARGET=%s", filename, target);

    system(command);

    return target;
}
