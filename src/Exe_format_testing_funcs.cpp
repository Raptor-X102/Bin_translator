#include "Exe_format_testing_funcs.h"

bool File_expansion_test(const char* filename1, const char* filename2) {

    size_t file_size = get_file_size(filename1);

    FILE* input_file = fopen(filename1, "rb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    char* file_buffer = (char*) calloc(file_size, sizeof(char));
    if(!file_buffer)  {

        DEBUG_PRINTF("ERROR: memory was not allocated\n");
        return false;
    }

    fread(file_buffer, sizeof(char), file_size, input_file);

    fclose(input_file);

    size_t index = 0;
    bool status = false;
    for(; index < file_size - sizeof(Main_search_instr) / sizeof(Main_search_instr[0]); index++) {

        if(!memcmp(file_buffer + index,
                   Main_search_instr,
                   sizeof(Main_search_instr) / sizeof(Main_search_instr[0]))) {

            status = true;
            break;
        }
    }

    if(!status) {

        DEBUG_PRINTF("ERROR: instructions were not found\n");
        return false;
    }

    FILE* output_file = fopen(filename2, "wb");
    if(!input_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    size_t search_buff_size = sizeof(Main_search_instr) / sizeof(Main_search_instr[0]);
    size_t add_buff_size = sizeof(String_to_add) / sizeof(String_to_add[0]);
    fwrite(file_buffer, sizeof(char), index + search_buff_size, output_file);
    fwrite(String_to_add, sizeof(String_to_add[0]),
           sizeof(String_to_add) / sizeof(String_to_add[0]), output_file);

    fwrite(file_buffer + index + search_buff_size + add_buff_size, sizeof(char),
           file_size - index - search_buff_size - add_buff_size, output_file);

    fclose(output_file);

    return true;
}
