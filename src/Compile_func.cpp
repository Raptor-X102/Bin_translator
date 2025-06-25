#include "Compile_func.h"

bool Compile_func(int argc, char* argv[]) {

    char** Input_files = Check_flags(argc, argv);
    if (!Input_files)
        return false;

    Buffer_data expression_buffer = {};
    Var_list var_list_functions = {};

    if (!Var_list_ctor(&var_list_functions, Var_list_init_size))
        return NULL;

    Func_data_list func_list = {.free_element = 0};
    Lexeme_array* lexeme_arr = NULL;
    Node* root = Lang_read_expression(Input_files[Src_file_index],
                                      &var_list_functions, &expression_buffer, &func_list,
                                      &lexeme_arr);

    if (!root) {

        DEBUG_PRINTF("ERROR: function returned NULL\n");
        return false;
    }

    if (Input_files[My_asm_file_index])
        if (!Compile_to_ASM(Input_files[My_asm_file_index], root, &func_list))
            return false;;

    if (Input_files[Dbg_file_index])
        if (!Tree_graph_debug(root, Input_files[Dbg_file_index], &Print_node_data))
            return false;

    if (Input_files[Asm_file_index])
        if (!Compile_nasm(Input_files[Asm_file_index], root, &func_list))
            return false;

    if (Input_files[Exe_file_index])
        if (!Compile_x64(IN_OUT_MODULES_PATH, Input_files[Exe_file_index], root, &func_list))
            return false;

/****************DTOR & FREE********************************** */
    Lexeme_array_dtor(lexeme_arr);
    for(int i = 0; i < func_list.free_element; i++) {

        Var_list_dtor(&func_list.func_data[i].local_vars);
        Var_list_dtor(&func_list.func_data[i].parameters);
    }

    for(int i = 0; i < File_types_total; i++) {

        if (Input_files[i])
            free(Input_files[i]);
    }

    free(Input_files);
    free(func_list.func_data);
    free(expression_buffer.buffer);
    Var_list_dtor(&var_list_functions);
/************************************************************ */
    return true;
}

char** Check_flags(int argc, char* argv[]) {

    char** Input_files = (char**) calloc(File_types_total, sizeof(char*));

    bool src_flag_found = false;
    bool option_flag_found = false;

    for(int i = 0; i < argc; i++) {

        if (!strncmp(argv[i], SRC_FLAG, SRC_FLAG_LEN)) {

            if (i + 1 < argc) {

                Input_files[Src_file_index] = strdup(argv[i+1]);
                src_flag_found = true;
            }

            else {

                DEBUG_PRINTF("ERROR: invalid input format\n");
                return NULL;
            }
        }

        if (!strncmp(argv[i], MY_ASM_FLAG, MY_ASM_FLAG_LEN)) {

            if (i + 1 < argc) {

                Input_files[My_asm_file_index] = strdup(argv[i+1]);
                option_flag_found = true;
            }

            else {

                DEBUG_PRINTF("ERROR: invalid input format\n");
                return NULL;
            }
        }

        if (!strncmp(argv[i], ASM_FLAG, ASM_FLAG_LEN)) {

            if (i + 1 < argc) {

                Input_files[Asm_file_index] = strdup(argv[i+1]);
                option_flag_found = true;
            }

            else {

                DEBUG_PRINTF("ERROR: invalid input format\n");
                return NULL;
            }
        }

        if (!strncmp(argv[i], EXE_FLAG, EXE_FLAG_LEN)) {

            if (i + 1 < argc) {

                Input_files[Exe_file_index] = strdup(argv[i+1]);
                option_flag_found = true;
            }

            else {

                DEBUG_PRINTF("ERROR: invalid input format\n");
                return NULL;
            }
        }

        if (!strncmp(argv[i], DBG_FLAG, DBG_FLAG_LEN)) {

            if (i + 1 < argc) {

                Input_files[Dbg_file_index] = strdup(argv[i+1]);
            }

            else {

                DEBUG_PRINTF("ERROR: invalid input format\n");
                return NULL;
            }
        }
    }

    if (!src_flag_found || !option_flag_found) {

        DEBUG_PRINTF("ERROR: invalid input format\n");
        return NULL;
    }

    return Input_files;
}

