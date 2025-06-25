#include "Create_main.h"

Func_instr_params* Create_main(const char* filename1, const char* filename2,
                               char** target_filename_ptr, char** buffer,
                               size_t* buffer_size_ptr, size_t nop_amount) {

    Func_instr_params* funcs_params = (Func_instr_params*) calloc(Funcs_total, sizeof(Func_instr_params));
    size_t file_size = get_file_size(filename1);

    FILE* input_file = fopen(filename1, "rb");
    if (!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    int ending_symbols_strlen = sizeof(Ending_symbols)-1;
    size_t buffer_size = ending_symbols_strlen + file_size;
    for(int i = 0; i < Base_asm_volatile_strings_amount; i++)
        buffer_size += ((nop_amount >> i) & 1) * Asm_strings_lens[i];

    for(int i = Base_asm_volatile_strings_amount; i < Instructions_bytes_limit; i++)
        buffer_size += ((nop_amount >> i) & 1) *
                        Asm_strings_lens[Base_asm_volatile_strings_amount-1] *
                        (2 << (i-Base_asm_volatile_strings_amount));

    char* file_buffer = (char*) calloc(buffer_size, sizeof(char));
    if (!file_buffer)  {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    fread(file_buffer, sizeof(char), file_size, input_file);
    file_buffer += file_size;
    fclose(input_file);

/**********************NAME MODIFICATIONS************************************************************************ */
    char* filename_buffer = (char*) calloc(strlen(filename2) + Filename_buffer_add, sizeof(char));
    char* dot_ptr = strchr(filename2, '.');
    if (!dot_ptr)
        sprintf(filename_buffer, "%s.cpp");

    else {

        int file_len = (size_t) (dot_ptr - filename2);
        memcpy(filename_buffer, filename2, file_len);
        memcpy(filename_buffer + file_len, ".cpp", sizeof(".cpp"));
    }
/***************************************************************************************************************** */

    FILE* output_file = fopen(filename_buffer, "wb");
    if (!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    size_t written_bytes = 0;
    for(int i = 0; i < Base_asm_volatile_strings_amount; i++) {

        if ((nop_amount & (1 << i))) {

            memcpy(file_buffer + written_bytes,
                   Asm_string_array[i],
                   Asm_strings_lens[i]);

            written_bytes += Asm_strings_lens[i];
        }
    }

    for(int i = Base_asm_volatile_strings_amount; i < Instructions_bytes_limit; i++) {

        if ((nop_amount & (1 << i))) {

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
    if (!file_buffer)  {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return NULL;
    }

    FILE* target_file = fopen(target, "rb");
    if (!target_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return NULL;
    }

    fread(file_buffer, sizeof(char), file_size, target_file);
    if (feof(target_file))
        DEBUG_PRINTF("EOF was reached\n");

    if (ferror(target_file))
        DEBUG_PRINTF("ERROR: fread failed\n");


    *buffer = file_buffer;

    INITIALIZE_FUNCS_PARAMS(funcs_params)
    if (!Get_all_funcs_offset(file_buffer, file_size, funcs_params))
        return NULL;

    int fclose_result = fclose(target_file);
    if (fclose_result != 0) {
        DEBUG_PRINTF("ERROR: file was not closed, returned %d\n", fclose_result);
        perror("fclose error");
    }

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
