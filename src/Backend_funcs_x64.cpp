#include "Backend_funcs_x64.h"

const char* Registers_names  [] =
                                  {"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
                                   "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15"};
const char* X_Registers_names[] =
                                  {"XMM0", "XMM1", "XMM2",  "XMM3",  "XMM4",  "XMM5",  "XMM6",  "XMM7",
                                   "XMM8", "XMM9", "XMM10", "XMM11", "XMM12", "XMM13", "XMM14", "XMM15"};

bool Compile_x64(const char* main_cpp_file,
                 const char* modified_main_cpp_file,
                 Node* root, Func_data_list* func_list) {

    Dynamic_array d_array_code = {};
    Dynamic_array_ctor(&d_array_code, D_array_code_init_size, NOP_OP);

    Dynamic_array d_array_funcs = {};
    Dynamic_array_ctor(&d_array_funcs, D_array_call_init_size, 0);

    Variable_data main = {};
    main.var_name = "main";
    main.var_len = strlen(main.var_name);
    Variable_data* main_ptr = &main;
    int64_t index = Find_func_data(func_list, &main_ptr);
    if(index == -1) {

        DEBUG_PRINTF("ERROR: 'main' function was not found\n");
        return false;
    }

    /*Func_data check_alignment = {};
    Variable_data* check_align_name = (Variable_data*) calloc(1, sizeof(Variable_data));
    check_align_name->var_name = Check_alignment_even;
    check_align_name->var_len = sizeof(Check_alignment_even) - 1;
    check_alignment.function = check_align_name;
    func_list->func_data[func_list->free_element] = check_alignment;
    func_list->free_element++;
    // = Insert_var(func_list, check_align_funcname, 0, 0);*/
    //DEBUG_PRINTF("CHECK FUNCS INSERTION STARTED\n");
    int64_t check_align_even_index = Insert_check_funcs(func_list, Check_alignment_even);
    int64_t check_align_odd_index  = Insert_check_funcs(func_list, Check_alignment_odd);
    //DEBUG_PRINTF("CHECK FUNCS INSERTION ENDED\n");
    //int32_t disp32 = 0;
    uint32_t fill_address = d_array_code.size + 1; // before we put jmp instr, + 1 call byte
    Dynamic_array_push_back(&d_array_funcs, &fill_address, sizeof(uint32_t));
    Dynamic_array_push_back(&d_array_funcs, &index, sizeof(int32_t));
    JMP_REL32(&d_array_code, No_jmp_rel32);
    //DEBUG_PRINTF("JMP ENDED\n");
    if(!Compile_user_function_def_x64(&d_array_code, &d_array_funcs, root, func_list))
        return false;

    DEBUG_PRINTF("Check_alignment_prologue_x64 STARTED\n");
    Check_alignment_prologue_x64(&d_array_code, func_list, check_align_even_index,
                                                           check_align_odd_index);
    DEBUG_PRINTF("Check_alignment_prologue_x64 ENDED\n");
    Func_instr_params* std_funcs = NULL;
    char* exe_file_name = NULL, * file_buffer = NULL;
    size_t buffer_size = 0;
    //DEBUG_PRINTF("d_array_code.size = %d\n", d_array_code.size - Main_prologue_length - Main_epilogue_length);
    //getchar();
    std_funcs =  Create_main(main_cpp_file, modified_main_cpp_file, &exe_file_name, &file_buffer, &buffer_size,
                             d_array_code.size - Main_prologue_length - Main_epilogue_length);

    DEBUG_PRINTF("Create_main ENDED\n");
    /*size_t entry_index = Find_main_entry(file_buffer, buffer_size, Bytes_before_fs_sf,
                         sizeof(Bytes_before_fs_sf) / sizeof(Bytes_before_fs_sf[0]),
                         Main_entry_offset_format_str);*/

    /*for(int i = 0; i < d_array_funcs.size; ) {

        int32_t dword = 0;
        memcpy(&dword, d_array_funcs.data + i, sizeof(int32_t));
        fprintf(stderr, "%08x ", dword);
        i += sizeof(int32_t);
        memcpy(&dword, d_array_funcs.data + i, sizeof(int32_t));
        fprintf(stderr, "%08x", dword);
        i += sizeof(int32_t);
        fprintf(stderr, "\n");
    }*/
    DEBUG_PRINTF("Fill_jmp_call_addresses STARTED\n");
    /*FILE* dump1 = fopen("dump1.bin", "wb");
    if(!dump1) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }
    for(int i = 0; i < d_array_code.size; i++) {

        uint8_t byte = d_array_code.data[i];
        fprintf(dump1, "%02x ", byte);
    }
    fprintf(dump1, "\n");
    fclose(dump1);*/
    Fill_jmp_call_addresses(&d_array_code, &d_array_funcs, func_list, std_funcs);
    /*FILE* dump2 = fopen("dump2.bin", "wb");
    if(!dump2) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }
    for(int i = 0; i < d_array_code.size; i++) {

        uint8_t byte = d_array_code.data[i];
        fprintf(dump2, "%02x ", byte);
    }
    fprintf(dump2, "\n");
    fclose(dump2);*/
    DEBUG_PRINTF("Fill_jmp_call_addresses ENDED\n");
    DEBUG_PRINTF("Find_main_entry STARTED\n");

    DEBUG_PRINTF("Find_main_entry ENDED\n");
    DEBUG_PRINTF("Memcpy_safe STARTED\n");
    Memcpy_safe(file_buffer + std_funcs[Main_entry_index].rel_address, d_array_code.data, d_array_code.size);
    DEBUG_PRINTF("Memcpy_safe ENDED\n");
    FILE* exe_output_file = fopen(exe_file_name, "wb");
    if(!exe_output_file) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    fwrite(file_buffer, sizeof(char), buffer_size, exe_output_file);
    fclose(exe_output_file);
    /******DEBUG******* */

    /*FILE* dump = fopen("Code_dump.txt", "wb");
    if(!dump) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    fwrite(d_array_code.data, sizeof(char), d_array_code.size, dump);
    fclose(dump);*/
    /****************** */
    Dynamic_array_dtor(&d_array_code);
    Dynamic_array_dtor(&d_array_funcs);
    return true;
}

void Fill_jmp_call_addresses(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                             Func_data_list* func_list, Func_instr_params* std_funcs) {

    DEBUG_PRINTF("d_array_funcs->size = %d\n", d_array_funcs->size);
    DEBUG_PRINTF("d_array_funcs->capacity = %d\n", d_array_funcs->capacity);
    assert(!(d_array_funcs->size % 8));
    for(uint32_t index = 0; index < d_array_funcs->size;) {

        uint32_t address_of_jmp_call = 0;
        int32_t dst_address = 0;
        memcpy(&address_of_jmp_call, &d_array_funcs->data[index], sizeof(uint32_t));
        index += sizeof(uint32_t);
        memcpy(&dst_address, &d_array_funcs->data[index], sizeof(int32_t));
        index += sizeof(uint32_t);

        if(dst_address == Format_string_sf_addr_index) {

            dst_address = std_funcs[Format_string_sf_index].rel_address -
                          address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }

        else if(dst_address == Format_string_pf_addr_index) {

            dst_address = std_funcs[Format_string_pf_index].rel_address -
                          address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }

        else if(dst_address == Scanf_func_index) {

            dst_address = std_funcs[Scanf_index].rel_address -
                          address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }

        else if(dst_address == Printf_func_index) {

            dst_address = std_funcs[Printf_index].rel_address -
                          address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }

        else if(dst_address < 0) {

            dst_address = -dst_address - address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }

        else {

            dst_address = func_list->func_data[dst_address].func_rel32_address -
                          address_of_jmp_call - Rel32_code_size;
            memcpy(&d_array_code->data[address_of_jmp_call], &dst_address, sizeof(int32_t));
        }
    }
}

void Check_alignment_prologue_x64(Dynamic_array* d_array_code, Func_data_list* func_list,
                              int64_t check_align_even_index, int64_t check_align_odd_index) {

    //while(d_array_code->size % 16 != 0) {

    INSERT_NOPS(d_array_code, d_array_code->size % 16);
    //}

    func_list->func_data[check_align_even_index].func_rel32_address = d_array_code->size;
    int alignment = 0x0F;
    MOV_REG_REG(d_array_code, I_RBX, I_RSP);
    //
    AND_REG_IMM(d_array_code, I_RBX, alignment);
    RET(d_array_code);

    INSERT_NOPS(d_array_code, d_array_code->size % 16);

    func_list->func_data[check_align_odd_index].func_rel32_address = d_array_code->size;
    MOV_REG_REG(d_array_code, I_RBX, I_RSP);
    NEG_REG(d_array_code, I_RBX);
    AND_REG_IMM(d_array_code, I_RBX, alignment);
    RET(d_array_code);
}

bool Compile_user_function_def_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                                   Node* root, Func_data_list* func_list) {

    DEBUG_PRINTF("Compile_user_function_def_x64 STARTED\n");
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == ANGLE_BRACKET_CL && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            if(!Compile_user_function_def_x64(d_array_code, d_array_funcs, root->left_node, func_list))
                return false;

        if(root->right_node)
            if(!Compile_user_function_def_x64(d_array_code, d_array_funcs, root->right_node, func_list))
               return false;
    }

    else {

        int64_t index = Find_func_data(func_list, &tmp_data->value);
        if(index == -1) {

            DEBUG_PRINTF("ERROR: this function was not inserted in func_data");
            return false;
        }

        //DEBUG_PRINTF("NOPS INSERTION STARTED\n");
        INSERT_NOPS(d_array_code, d_array_code->size % 16);

        func_list->func_data[index].func_rel32_address = d_array_code->size;
        //Dynamic_array_push_back(d_array_funcs, &d_array_code->size, sizeof(uint32_t));
        //DEBUG_PRINTF("NOPS INSERTION ENDED\n");
        /*PRINTOUT("%.*s:\n", (size_t) func_list->func_data[index].function->var_len,
                                     func_list->func_data[index].function->var_name);
        if(func_list->func_data[index].parameters.free_var > 0)
            for(int i = .free_var - 1; i >= 0; i--)
                PRINTOUT("POP [BX+%d]\n", i);*/

        PUSH_REG(d_array_code, I_RBP);
        MOV_REG_REG(d_array_code, I_RBP, I_RSP);
        int32_t stack_frame = (func_list->func_data[index].local_vars.var_list_size +
                               (func_list->func_data[index].local_vars.var_list_size % 2))* Value_size ;
        /*int32_t stack_frame = Shadow_space +
                              8 * func_list->func_data[index].parameters.var_list_size;
                              //8 * func_list->func_data[index].local_vars.var_list_size;
        size_t loc_vars = func_list->func_data[index].local_vars.var_list_size;
        if(loc_vars > (Shadow_space / 8))
            stack_frame += (loc_vars - (Shadow_space / 8)) * 8;*/



        SUB_REG_IMM(d_array_code, I_RSP, stack_frame);
        //SUB_REG_REG(d_array_code, I_RSP, I_RBP);


        if(!Compile_operation_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
            return false;
    }

    DEBUG_PRINTF("Compile_user_function_def_x64 ENDED\n");
    return true;
}

bool Compile_operation_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                           Func_data_list* func_list, int64_t index) {

    DEBUG_PRINTF("Compile_operation_x64 STARTED\n");
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    DEBUG_PRINTF("root = %p\t", root);
    DEBUG_PRINTF("tmp_data->value = %x\t", tmp_data->value);
    DEBUG_PRINTF("tmp_data->expression_type = %x\n", tmp_data->expression_type);
    if(tmp_data->value == TAB && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node) {

            DEBUG_PRINTF("ENTERED COMPILING left_node\n");
            if(!Compile_operation_x64(d_array_code, d_array_funcs, root->left_node, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING left_node\n");
        }
        if(root->right_node) {

            DEBUG_PRINTF("ENTERED COMPILING right_node\n");
            if(!Compile_operation_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING right_node\n");
        }
    }

    else {

        if(tmp_data->value == IF && tmp_data->expression_type == KEY_WORD) {

            DEBUG_PRINTF("ENTERED COMPILING **IF**\n");
            if(!Compile_if_x64(d_array_code, d_array_funcs, root, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING **IF**\n");
        }

        else if(tmp_data->value == WHILE && tmp_data->expression_type == KEY_WORD) {

            DEBUG_PRINTF("ENTERED COMPILING **WHILE**\n");
            if(!Compile_while_x64(d_array_code, d_array_funcs, root, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING **WHILE**\n");
        }

        else if(tmp_data->value == ASSIGNMENT && tmp_data->expression_type == SPECIAL_SYMBOL) {

            DEBUG_PRINTF("ENTERED COMPILING **ASSIGNMENT**\n");
            if(!Compile_assignment_x64(d_array_code, d_array_funcs, root, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING **ASSIGNMENT**\n");
        }

        else if(tmp_data->expression_type == FUNCTION) {

            DEBUG_PRINTF("ENTERED COMPILING **FUNCTION**\n");
            if(!Compile_user_function_x64(d_array_code, d_array_funcs, root, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING **FUNCTION**\n");
        }

        else if(tmp_data->value == RETURN && tmp_data->expression_type == KEY_WORD) {

            DEBUG_PRINTF("ENTERED COMPILING **RETURN**\n");
            if(!Compile_return_x64(d_array_code, d_array_funcs, root, func_list, index))
                return false;
            DEBUG_PRINTF("EXITED COMPILING **RETURN**\n");
        }

        else {

            DEBUG_PRINTF("ERROR: unknown operation\n");
            Variable_data* var_data = NULL;
            memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
            DEBUG_PRINTF("VAR = %.*s", var_data->var_len, var_data->var_name);
            return false;
        }
    }

    DEBUG_PRINTF("Compile_operation_x64 ENDED\n");
    return true;
}

bool Compile_while_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                       Func_data_list* func_list, int64_t index) {

    DEBUG_PRINTF("Compile_while_x64 STARTED\n");
    int64_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;
    /*PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond:\n", while_count_copy);*/
    int32_t cond_jump_address = -d_array_code->size;
    uint32_t end_loop_jump_index = Compile_condition_x64(d_array_code, d_array_funcs, root->left_node, func_list, index, Direct_option);
    if(end_loop_jump_index == Error_value)
        return false;
    /*PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", while_count_copy);
    PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", while_count_copy);

    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", while_count_copy);*/
    if(root->right_node)
        if(!Compile_operation_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
            return false;

    uint32_t fill_address = d_array_code->size + 1; // before we put jmp instr, + 1 call byte
    int32_t fill_jmp_disp = 0;
    Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
    Dynamic_array_push_back(d_array_funcs, &cond_jump_address, sizeof(int32_t));
    JMP_REL32(d_array_code, fill_jmp_disp);
    int32_t end_loop_jump_address = -d_array_code->size;
    memcpy(d_array_funcs->data + end_loop_jump_index, &end_loop_jump_address, sizeof(int32_t));
    /*PRINTOUT("JMP %.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_check_cond\n", while_count_copy);
    PRINTOUT("%.*s_while_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n\n", while_count_copy);*/
    DEBUG_PRINTF("Compile_while_x64 ENDED\n");
    return true;
}

bool Compile_if_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root,
                    Func_data_list* func_list, int64_t index) {

    DEBUG_PRINTF("Compile_if_x64 STARTED\n");
    Node* condition_root = root->left_node, * body_node = root->right_node;
    uint32_t fill_address = 0;
    int32_t fill_jmp_disp = 0;
    uint32_t cond_jump_index = Compile_condition_x64(d_array_code, d_array_funcs, condition_root, func_list, index, Direct_option);
    if(cond_jump_index == Error_value)
        return false;

    int32_t var_len = func_list->func_data[index].function->var_len;
    char* var_name = func_list->func_data[index].function->var_name;

    /*PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes\n", if_count_copy);
    PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no\n\n", if_count_copy);

    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes:\n", if_count_copy);*/
    if(body_node->left_node)
        if(!Compile_operation_x64(d_array_code, d_array_funcs, body_node->left_node, func_list, index))
            return false;

    if(body_node->right_node) {

        fill_address = d_array_code->size + 1; // before we put jmp instr, + 1 call byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &fill_jmp_disp, sizeof(int32_t));
        JMP_REL32(d_array_code, No_jmp_rel32);
    }
    fill_jmp_disp = -d_array_code->size;
    memcpy(d_array_funcs->data + cond_jump_index, &fill_jmp_disp, sizeof(uint32_t));
    cond_jump_index = d_array_funcs->size - sizeof(int32_t);

    /*PRINTOUT("JMP %.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret\n", if_count_copy);
    PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_no:\n", if_count_copy);*/
    if(body_node->right_node) {
        if(!Compile_operation_x64(d_array_code, d_array_funcs, body_node->right_node, func_list, index))
            return false;

        fill_jmp_disp = -d_array_code->size;
        memcpy(d_array_funcs->data + cond_jump_index, &fill_jmp_disp, sizeof(uint32_t));
    }
    /*PRINTOUT("%.*s_if_", (size_t) var_len, var_name);
    PRINTOUT("%d_yes_ret:\n", if_count_copy);*/
    DEBUG_PRINTF("Compile_if_x64 ENDED\n");
    return true;
}

uint32_t Compile_condition_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                               Node* root, Func_data_list* func_list, int64_t index, int option) {

    DEBUG_PRINTF("Compile_condition_x64 STARTED\n");
    if(!Compile_operator_x64(d_array_code, d_array_funcs, root->left_node, func_list, index))
        return Error_value;
    if(!Compile_operator_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
        return Error_value;

    int32_t disp32 = 0, imm32 = Value_size;
    MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM(d_array_code, I_RSP, imm32);
    MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM1, I_RSP);
    ADD_REG_IMM(d_array_code, I_RSP, imm32);
    COMISD_XREG_XREG(d_array_code, I_XMM1, I_XMM0);

    /*
    uint32_t fill_address = d_array_code->size + 2; // before we put jmp instr, + 2 cond jmp byte
    Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
    Dynamic_array_push_back(d_array_funcs, &disp32, sizeof(int32_t));
    */
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    #define DEFINE_JXX(value, JXX) case value:\
                                        JXX##_REL8(d_array_code, Skip_5_bytes);\
                                        break;

    if(option == Direct_option) {

        switch(tmp_data->value) {

            #include "Conditions_code_gen_x64.h"

            default:
                DEBUG_PRINTF("ERROR: no such condition\n");
                return Error_value;
        }
    }

    else {

        switch(tmp_data->value) {

            #include "Conditions_code_gen_reverse_x64.h"

            default:
                DEBUG_PRINTF("ERROR: no such condition\n");
                return Error_value;
        }
    }

    #undef DEFINE_JXX
    uint32_t fill_address = d_array_code->size + 1; // before we put jmp instr, + 2 cond jmp byte
    Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
    Dynamic_array_push_back(d_array_funcs, &disp32, sizeof(int32_t));
    JMP_REL32(d_array_code, No_jmp_rel32);

    return d_array_funcs->size - sizeof(int32_t);
}

bool Compile_return_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                        int64_t index) {

    DEBUG_PRINTF("Compile_return_x64 STARTED\n");
    if(root->right_node)
        if(!Compile_operator_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
            return false;

    int32_t disp32 = 0, imm32 = Value_size;
    MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM(d_array_code, I_RSP, imm32);
    LEAVE(d_array_code);
    RET(d_array_code);
    DEBUG_PRINTF("Compile_return_x64 ENDED\n");
    return true;
}

bool Compile_assignment_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                            int64_t index) {

    DEBUG_PRINTF("Compile_assignment_x64 STARTED\n");
    Node_data* node_data_left = NULL;
    Variable_data* var_data = NULL;
    int32_t disp32 = 0, imm32 = 8;

    //assert(root);
    //assert(root->right_node);
    DEBUG_PRINTF("root = %p\n", root);
    DEBUG_PRINTF("root->right_node = %p\n", root->right_node);
    Compile_operator_x64(d_array_code, d_array_funcs, root->right_node, func_list, index);
    MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM(d_array_code, I_RSP, imm32);

    assert(root->left_node);
    DEBUG_PRINTF("root->left_node->node_data = %p", root->left_node->node_data);
    memcpy(&node_data_left, &root->left_node->node_data, sizeof(Node_data*));
    DEBUG_PRINTF("\n");
    memcpy(&var_data, &node_data_left->value, sizeof(Variable_data*));
    int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                      var_data->var_name, var_data->var_len);

    if(var_index == -1) {

        //int64_t loc_var_mem_index = func_list->func_data[index].parameters.free_var;
        var_index = Find_variable(&func_list->func_data[index].parameters,
                                  var_data->var_name, var_data->var_len);
        if(var_index == -1) {

            DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
            return false;
        }
        int32_t disp32 = ((var_index + 2)* 8);    // +1 due to return address & RBP in stack
        MOVSD_MEM_XREG(d_array_code, I_RBP, I_XMM0, disp32);
        //PUSH_RBP_DISP32(d_array_code, disp32);
        //PRINTOUT("POP [BX+%d]\n", var_index + loc_var_mem_index);
    }

    else {

        int32_t disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
        MOVSD_MEM_XREG(d_array_code, I_RBP, I_XMM0, disp32);
        //PRINTOUT("POP [BX+%d]\n", var_index);
    }

    DEBUG_PRINTF("Compile_assignment_x64 ENDED\n");
    return true;
}

bool Compile_operator_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                          int64_t index) {

    DEBUG_PRINTF("Compile_operator_x64 STARTED\n");
    Node_data* tmp_data = NULL;
    int32_t disp32 = 0, imm32 = 0;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));

    if(tmp_data->expression_type == OP) {

        if(root->left_node)
            if(!Compile_operator_x64(d_array_code, d_array_funcs, root->left_node, func_list, index))
                return false;

        if(root->right_node)
            if(!Compile_operator_x64(d_array_code, d_array_funcs, root->right_node, func_list, index))
                return false;

        disp32 = 0;
        imm32 = Value_size;

        //ADD_REG_IMM(d_array_code, I_RSP, imm32);

        switch(tmp_data->value) {

            #define FUNC_BIN(value, symbol) case value: {\
                                             MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM0, I_RSP);\
                                             ADD_REG_IMM(d_array_code, I_RSP, imm32);\
                                             MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM1, I_RSP);\
                                             value##SD_XREG_XREG(d_array_code, I_XMM1, I_XMM0);\
                                             break;\
                                        }

            #define FUNC_UN(value, symbol) case value: {\
                                             MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM1, I_RSP);\
                                             value##SD_XREG_XREG(d_array_code, I_XMM1, I_XMM1);\
                                             break;\
                                        }


            #include "Funcs_code_gen_x64.h"
            #undef FUNC_BIN
            #undef FUNC_UN

            case ABS: {

                MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM1, I_RSP);
                MOV_REG_IMM64(d_array_code, I_RDX, Double_sign_mask);
                PUSH_REG(d_array_code, I_RDX);
                MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM0, I_RSP);
                ADD_REG_IMM(d_array_code, I_RSP, imm32);
                ANDPD_XREG_XREG(d_array_code, I_XMM1, I_XMM0);
                break;
            }

            default:
                DEBUG_PRINTF("ERROR: no such function\n");
                return false;
        }

        MOVSD_MEM_XREG_DISP0(d_array_code, I_RSP, I_XMM1);
    }

    else if(tmp_data->expression_type == VAR) {

        Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);

        if(var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);
            if(var_index == -1) {

                DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
                return false;
            }
            disp32 = (var_index + 2)* 8;    // +2 due to return address & rbp in stack
            //MOVSD_XREG_MEM()
            PUSH_RBP_DISP32(d_array_code, disp32);
        }

        else {

            //int64_t loc_var_mem_index = func_list->func_data[index].parameters.free_var;
            disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
            PUSH_RBP_DISP32(d_array_code, disp32);
            //PRINTOUT("PUSH [BX+%d]\n", var_index + loc_var_mem_index);
        }
    }

    else if(tmp_data->expression_type == NUM) {

        DEBUG_PRINTF("INSERTING NUMBER\n");
        double tmp_double = 0;
        memcpy(&tmp_double, &tmp_data->value, sizeof(double));
        MOV_REG_IMM64(d_array_code, I_RDX, tmp_double);
        PUSH_REG(d_array_code, I_RDX);
        //PRINTOUT("PUSH %lg\n", tmp_double);
    }

    else if(tmp_data->expression_type == FUNCTION) {

        if(!Compile_user_function_x64(d_array_code, d_array_funcs, root, func_list, index))
            return false;
    }

    else {

        DEBUG_PRINTF("ERROR: unknown operator\n");
        /*Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        DEBUG_PRINTF("tmp_data->expression_type = %d\n", tmp_data->expression_type);
        DEBUG_PRINTF("VAR = %.*s\n", var_data->var_len, var_data->var_name);*/
        return false;
    }

    DEBUG_PRINTF("Compile_operator_x64 ENDED\n");
    return true;
}

bool Compile_user_function_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs,
                               Node* root, Func_data_list* func_list, int64_t index) {

    DEBUG_PRINTF("Compile_user_function_x64 STARTED\n");
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    Variable_data* var_data = NULL;
    memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
    if(!strncmp(var_data->var_name, "Out", var_data->var_len)) {

        int32_t disp32 = 0, imm32 = 56;
        int parameters_pushed = 0;
        Compile_push_parameters_x64(d_array_code, d_array_funcs, root->left_node, func_list, index, &parameters_pushed);
        if(parameters_pushed != 1) {

            DEBUG_PRINTF("ERROR: in func 'Out' wrong arguments amount\n");
            return false;
        }
        MOVSD_XREG_MEM_DISP0(d_array_code, I_XMM1, I_RSP);
        MOV_REG_MEM_DISP0(d_array_code, I_RDX, I_RSP);
        SUB_REG_IMM(d_array_code, I_RSP, imm32); // need to allocate 64 bytes = 32 (shadow space) + 32 (space for xmm0-xmm3),
                                                 // but 8 bytes already pushed as a parameter, so 56 bytes
        //disp32 = 0x18;
        //MOVSD_MEM_XREG_DISP0(d_array_code, I_RSP, I_XMM0, ); // windows calling conventions for debug
        int64_t imm64 = 1;
        MOV_REG_IMM64(d_array_code, I_RAX, imm64);
        uint32_t fill_address = d_array_code->size + 3; // before we put lea, + 3 lea byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &Format_string_pf_addr_index, sizeof(int32_t));

        LEA_REG_RIP_REL32(d_array_code, I_RCX, No_jmp_rel32);

        fill_address = d_array_code->size + 1; // before we put call, + 1 call instr byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &Printf_func_index, sizeof(int32_t));

        CALL_REL32(d_array_code, No_jmp_rel32);

        imm32 = 64;
        ADD_REG_IMM(d_array_code, I_RSP, imm32);
    }

    else if(!strncmp(var_data->var_name, "In", var_data->var_len)) {

        int32_t disp32 = 0, imm32 = 64;
        SUB_REG_IMM(d_array_code, I_RSP, imm32);

        uint32_t fill_address = d_array_code->size + 3; // before we put lea, + 3 lea byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &Format_string_sf_addr_index, sizeof(int32_t));
        LEA_REG_RIP_REL32(d_array_code, I_RCX, No_jmp_rel32);

        /*********Get parameter***********************************/

        if(!root->left_node) {

            DEBUG_PRINTF("ERROR: in func 'In' wrong arguments amount\n");
            return false;
        }

        memcpy(&tmp_data, &root->left_node->node_data, sizeof(Node_data*));
        Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);

        if(var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);

            if(var_index == -1) {

                DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
                return false;
            }
            disp32 = (var_index + 2)* 8;    // +1 due to return address & RBP in stack
            LEA_REG_MEM(d_array_code, I_RDX, I_RBP, disp32);
        }

        else {

            disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
            LEA_REG_MEM(d_array_code, I_RDX, I_RBP, disp32);
        }
        /************************************************************** */

        fill_address = d_array_code->size + 1; // before we put call, + 1 call instr byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &Scanf_func_index, sizeof(int32_t));

        CALL_REL32(d_array_code, No_jmp_rel32);

        ADD_REG_IMM(d_array_code, I_RSP, imm32);
    }

    else {

        int64_t callee_index = Find_func_data(func_list, &var_data);
        if(callee_index == -1) {

            DEBUG_PRINTF("ERROR: '%.*s' function was not found\n", (size_t) var_data->var_len, var_data->var_name);
            return false;
        }

        Variable_data check_alignment = {};
        int64_t parameters = (func_list->func_data[callee_index].parameters.free_var);
        if(!(parameters % 2)) {

            check_alignment.var_name = strdup(Check_alignment_even);
            check_alignment.var_len = sizeof(Check_alignment_even) - 1;
        }

        else {

            check_alignment.var_name = strdup(Check_alignment_odd);
            check_alignment.var_len = sizeof(Check_alignment_odd) - 1;
        }

        Variable_data* check_alignment_ptr = &check_alignment;
        int64_t check_alignment_index = Find_func_data(func_list, &check_alignment_ptr);
        if(check_alignment_index == -1) {

            DEBUG_PRINTF("ERROR: 'check_alignment_ptr' function was not found\n");
            return false;
        }

        uint32_t fill_address = d_array_code->size + 1; // before we put jmp instr, + 1 call byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &check_alignment_index, sizeof(int32_t));
        CALL_REL32(d_array_code, No_jmp_rel32);
        int parameters_pushed = 0;
        SUB_REG_REG(d_array_code, I_RSP, I_RBX);
        Compile_push_parameters_x64(d_array_code, d_array_funcs, root->left_node, func_list, index, &parameters_pushed);

        fill_address = d_array_code->size + 1; // before we put jmp instr, + 1 call byte
        Dynamic_array_push_back(d_array_funcs, &fill_address, sizeof(uint32_t));
        Dynamic_array_push_back(d_array_funcs, &callee_index, sizeof(int32_t));
        CALL_REL32(d_array_code, No_jmp_rel32);
        ADD_REG_REG(d_array_code, I_RSP, I_RBX);
        if(Value_size*(parameters-1))
            ADD_REG_IMM(d_array_code, I_RSP, Value_size*(parameters-1)); // need -1 to push into stack returning value
        //SUB_REG_IMM(d_array_code, I_RSP, Value_size);
        int32_t disp32 = 0;
        MOVSD_MEM_XREG_DISP0(d_array_code, I_RSP, I_XMM0);
    }

    DEBUG_PRINTF("Compile_user_function_x64 ENDED\n");
    return true;
}

bool Compile_push_parameters_x64(Dynamic_array* d_array_code, Dynamic_array* d_array_funcs, Node* root, Func_data_list* func_list,
                                 int64_t index, int* parameters_pushed) {

    DEBUG_PRINTF("Compile_push_parameters_x64 STARTED\n");
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if(tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if(root->left_node)
            if(!Compile_push_parameters_x64(d_array_code, d_array_funcs, root->left_node, func_list, index, parameters_pushed))
                return false;

        if(root->right_node)
            if(!Compile_push_parameters_x64(d_array_code, d_array_funcs, root->right_node, func_list, index, parameters_pushed))
                return false;
    }

    else { // push s konca

        /*Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);

        if(var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);
            int32_t disp32 = (var_index + 1)* 8;    // +1 due to return address in stack
            PUSH_RBP_DISP32(d_array_code, disp32);
            //PRINTOUT("PUSH [BX+%d]\n", var_index);
        }

        else {

            int32_t disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
            PUSH_RBP_DISP32(d_array_code, disp32);
            //int64_t loc_var_mem_index = func_list->func_data->parameters.free_var;
            //PRINTOUT("PUSH [BX+%d]\n", var_index + loc_var_mem_index);
        }*/
        if(!Compile_operator_x64(d_array_code, d_array_funcs, root, func_list, index))
            return false;

        *parameters_pushed += 1;
    }

    DEBUG_PRINTF("Compile_push_parameters_x64 ENDED\n");
    return true;
}
