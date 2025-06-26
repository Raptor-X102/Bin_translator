#include "Backend_funcs_nasm.h"

bool Compile_nasm(const char* output_file_name_nasm,
                 Node* root, Func_data_list* func_list) {

    Dynamic_array d_array_code = {};
    Dynamic_array_ctor(&d_array_code, D_array_code_init_size, 0); // fillbyte = 0

    Variable_data main = {};
    main.var_name = "main";
    main.var_len = strlen(main.var_name);
    Variable_data* main_ptr = &main;
    int64_t index = Find_func_data(func_list, &main_ptr);
    if (index == -1) {

        DEBUG_PRINTF("ERROR: 'main' function was not found\n");
        return false;
    }

    INSERT_STRING_CONSTANT(&d_array_code, Prologue_data_nasm);
    INSERT_STRING_CONSTANT(&d_array_code, Prologue_text_nasm);

    int local_labels_count = 0;
    if (!Compile_user_function_def_nasm(&d_array_code, root, func_list, &local_labels_count))
        return false;

    Check_alignment_prologue_nasm(&d_array_code, func_list);

    FILE* output_file_nasm = fopen(output_file_name_nasm, "wb");
    if (!output_file_nasm) {

        DEBUG_PRINTF("ERROR: file was not opened\n");
        return false;
    }

    fwrite(d_array_code.data, sizeof(char), d_array_code.size, output_file_nasm);
    fclose(output_file_nasm);

    Dynamic_array_dtor(&d_array_code);
    return true;
}

void Check_alignment_prologue_nasm(Dynamic_array* d_array_code, Func_data_list* func_list) {

    int symbols_written = 0;
    int alignment = 0x0F;

    INSERT_STRING_CONSTANT(d_array_code, "\nCheck_alignment_even:\n"); \
    MOV_REG_REG_NASM(d_array_code, I_RBX, I_RSP);
    AND_REG_IMM_NASM(d_array_code, I_RBX, alignment);
    RET_NASM(d_array_code);

    INSERT_STRING_CONSTANT(d_array_code, "\nCheck_alignment_odd:\n"); \
    MOV_REG_REG_NASM(d_array_code, I_RBX, I_RSP);
    NEG_REG_NASM(d_array_code, I_RBX);
    AND_REG_IMM_NASM(d_array_code, I_RBX, alignment);
    RET_NASM(d_array_code);
}

bool Compile_user_function_def_nasm(Dynamic_array* d_array_code,Node* root, Func_data_list* func_list,
                                    int* local_labels_count) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if (tmp_data->value == ANGLE_BRACKET_CL && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if (root->left_node) {

            int local_labels_count_left = 0;
            if (!Compile_user_function_def_nasm(d_array_code, root->left_node, func_list, &local_labels_count_left))
                return false;
        }

        if (root->right_node) {

            int local_labels_count_right = 0;
            if (!Compile_user_function_def_nasm(d_array_code, root->right_node, func_list, &local_labels_count_right))
               return false;
        }
    }

    else {

        int64_t index = Find_func_data(func_list, &tmp_data->value);
        if (index == -1) {

            DEBUG_PRINTF("ERROR: this function was not inserted in func_data");
            return false;
        }

        INSERT_STRING_CONSTANT(d_array_code, "\n");
        INSERT_TEXT(d_array_code, func_list->func_data[index].function->var_name,
                    func_list->func_data[index].function->var_len);
        INSERT_STRING_CONSTANT(d_array_code, ":\n");

        int symbols_written = 0;
        int32_t stack_frame = (func_list->func_data[index].local_vars.var_list_size +
                               (func_list->func_data[index].local_vars.var_list_size % 2))* Value_size ;
        PUSH_REG_NASM(d_array_code, I_RBP);
        MOV_REG_REG_NASM(d_array_code, I_RBP, I_RSP);
        SUB_REG_IMM_NASM(d_array_code, I_RSP, stack_frame);

        if (!Compile_operation_nasm(d_array_code, root->right_node, func_list, index, local_labels_count))
            return false;
    }

    return true;
}

bool Compile_operation_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                            int64_t index, int* local_labels_count) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if (tmp_data->value == TAB && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if (root->left_node) {

            if (!Compile_operation_nasm(d_array_code, root->left_node, func_list, index, local_labels_count))
                return false;
        }

        if (root->right_node) {

            if (!Compile_operation_nasm(d_array_code, root->right_node, func_list, index, local_labels_count))
                return false;
        }
    }

    else {

        if (tmp_data->value == IF && tmp_data->expression_type == KEY_WORD) {

            if (!Compile_if_nasm(d_array_code, root, func_list, index, local_labels_count))
                return false;
        }

        else if (tmp_data->value == WHILE && tmp_data->expression_type == KEY_WORD) {

            if (!Compile_while_nasm(d_array_code, root, func_list, index, local_labels_count))
                return false;
        }

        else if (tmp_data->value == ASSIGNMENT && tmp_data->expression_type == SPECIAL_SYMBOL) {

            if (!Compile_assignment_nasm(d_array_code, root, func_list, index))
                return false;
        }

        else if (tmp_data->expression_type == FUNCTION) {

            if (!Compile_user_function_nasm(d_array_code, root, func_list, index))
                return false;
        }

        else if (tmp_data->value == RETURN && tmp_data->expression_type == KEY_WORD) {

            if (!Compile_return_nasm(d_array_code, root, func_list, index))
                return false;
        }

        else {

            DEBUG_PRINTF("ERROR: unknown operation\n");
            return false;
        }
    }

    return true;
}

bool Compile_while_nasm(Dynamic_array* d_array_code, Node* root,
                       Func_data_list* func_list, int64_t index, int* local_labels_count) {

    int symbols_written = 0;
    int local_labels_count_cpy = *local_labels_count;
    (*local_labels_count)++;
    INSERT_LABEL(d_array_code, local_labels_count_cpy);

    if (!Compile_condition_nasm(d_array_code, root->left_node, func_list, index,
                              Direct_option, local_labels_count))
        return false;

    INSERT_LABEL(d_array_code, local_labels_count_cpy + 1);

    if (root->right_node)
        if (!Compile_operation_nasm(d_array_code, root->right_node, func_list, index, local_labels_count))
            return false;

    JMP_NASM(d_array_code, local_labels_count_cpy);
    INSERT_LABEL(d_array_code, local_labels_count_cpy + 2);

    return true;
}

bool Compile_if_nasm(Dynamic_array* d_array_code, Node* root,
                    Func_data_list* func_list, int64_t index, int* local_labels_count) {

    int symbols_written = 0;
    Node* condition_root = root->left_node, * body_node = root->right_node;
    if (!Compile_condition_nasm(d_array_code, condition_root, func_list, index,
                              Direct_option, local_labels_count))
        return false;

    INSERT_LABEL(d_array_code, *(local_labels_count) - 2);
    int local_labels_count_cpy = *local_labels_count;

    if (body_node->left_node)
        if (!Compile_operation_nasm(d_array_code, body_node->left_node, func_list,
                                  index, local_labels_count))
            return false;

    if (body_node->right_node) {

        JMP_NASM(d_array_code, (*local_labels_count));
        (*local_labels_count)++;
    }

    INSERT_LABEL(d_array_code, local_labels_count_cpy - 1);
    local_labels_count_cpy = (*local_labels_count);

    if (body_node->right_node) {

        if (!Compile_operation_nasm(d_array_code, body_node->right_node, func_list,
                                  index, local_labels_count))
            return false;

        INSERT_LABEL(d_array_code, local_labels_count_cpy - 1);
    }

    return true;
}

bool Compile_condition_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list, int64_t index,
                            int option, int* local_labels_count) {

    if (root->left_node)
        if (!Compile_operator_nasm(d_array_code, root->left_node, func_list, index))
            return Error_value;

    if (root->right_node)
        if (!Compile_operator_nasm(d_array_code, root->right_node, func_list, index))
            return Error_value;

    int32_t disp32 = 0, imm32 = Value_size;
    int symbols_written = 0;
    MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);
    MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM1, I_RSP);
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);
    COMISD_XREG_XREG_NASM(d_array_code, I_XMM1, I_XMM0);

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    #define DEFINE_JXX(value, JXX) case value:\
                                        JXX##_NASM(d_array_code, *local_labels_count);\
                                        break;

    if (option == Direct_option) {

        switch (tmp_data->value) {

            #include "Conditions_code_gen_x64.h"

            default:
                DEBUG_PRINTF("ERROR: no such condition\n");
                return false;
        }
    }

    else {

        switch (tmp_data->value) {

            #include "Conditions_code_gen_reverse_x64.h"

            default:
                DEBUG_PRINTF("ERROR: no such condition\n");
                return false;
        }
    }

    #undef DEFINE_JXX

    (*local_labels_count)++;
    JMP_NASM(d_array_code, *local_labels_count);
    (*local_labels_count)++;

    return true;
}

bool Compile_return_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                        int64_t index) {

    if (root->right_node)
        if (!Compile_operator_nasm(d_array_code, root->right_node, func_list, index))
            return false;

    int32_t disp32 = 0, imm32 = Value_size;
    int symbols_written = 0;
    MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);
    LEAVE_NASM(d_array_code);
    RET_NASM(d_array_code);

    return true;
}

bool Compile_assignment_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                            int64_t index) {

    Node_data* node_data_left = NULL;
    Variable_data* var_data = NULL;
    int32_t disp32 = 0, imm32 = 8;
    int symbols_written = 0;

    Compile_operator_nasm(d_array_code, root->right_node, func_list, index);
    MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM0, I_RSP);
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);

    assert(root->left_node);
    memcpy(&node_data_left, &root->left_node->node_data, sizeof(Node_data*));
    memcpy(&var_data, &node_data_left->value, sizeof(Variable_data*));
    int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                      var_data->var_name, var_data->var_len);

    if (var_index == -1) {

        var_index = Find_variable(&func_list->func_data[index].parameters,
                                  var_data->var_name, var_data->var_len);
        if (var_index == -1) {

            DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
            return false;
        }
        int32_t disp32 = ((var_index + 2)* 8);    // +1 due to return address & RBP in stack
        MOVSD_MEM_XREG_NASM(d_array_code, I_RBP, I_XMM0, disp32);
    }

    else {

        int32_t disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
        MOVSD_MEM_XREG_NASM(d_array_code, I_RBP, I_XMM0, disp32);
    }

    return true;
}

bool Compile_operator_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                          int64_t index) {

    Node_data* tmp_data = NULL;
    int32_t disp32 = 0, imm32 = 0;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    int symbols_written = 0;
    if (tmp_data->expression_type == OP) {

        if (root->left_node)
            if (!Compile_operator_nasm(d_array_code, root->left_node, func_list, index))
                return false;

        if (root->right_node)
            if (!Compile_operator_nasm(d_array_code, root->right_node, func_list, index))
                return false;

        disp32 = 0;
        imm32 = Value_size;

        switch (tmp_data->value) {

            #define FUNC_BIN(value, symbol) case value: {\
                                             MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM0, I_RSP);\
                                             ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);\
                                             MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM1, I_RSP);\
                                             value##SD_XREG_XREG_NASM(d_array_code, I_XMM1, I_XMM0);\
                                             break;\
                                        }

            #define FUNC_UN(value, symbol) case value: {\
                                             MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM1, I_RSP);\
                                             value##SD_XREG_XREG_NASM(d_array_code, I_XMM1, I_XMM1);\
                                             break;\
                                        }


            #include "Funcs_code_gen_x64.h"
            #undef FUNC_BIN
            #undef FUNC_UN

            case ABS: {

                MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM1, I_RSP);
                MOV_REG_IMM64_NASM(d_array_code, I_RDX, Double_sign_mask);
                PUSH_REG_NASM(d_array_code, I_RDX);
                MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM0, I_RSP);
                ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);
                ANDPD_XREG_XREG_NASM(d_array_code, I_XMM1, I_XMM0);
                break;
            }

            default:
                DEBUG_PRINTF("ERROR: no such function\n");
                return false;
        }

        MOVSD_MEM_XREG_DISP0_NASM(d_array_code, I_RSP, I_XMM1);
    }

    else if (tmp_data->expression_type == VAR) {

        Variable_data* var_data = NULL;
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
        int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                        var_data->var_name, var_data->var_len);

        if (var_index == -1) {

            var_index = Find_variable(&func_list->func_data[index].parameters,
                                    var_data->var_name, var_data->var_len);
            if (var_index == -1) {

                DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
                return false;
            }
            disp32 = (var_index + 2)* 8;    // +2 due to return address & rbp in stack
            PUSH_RBP_DISP32_NASM(d_array_code, disp32);
        }

        else {

            disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
            PUSH_RBP_DISP32_NASM(d_array_code, disp32);
        }
    }

    else if (tmp_data->expression_type == NUM) {

        double tmp_double = 0;
        memcpy(&tmp_double, &tmp_data->value, sizeof(double));
        MOV_REG_IMM64_NASM(d_array_code, I_RDX, tmp_double);
        PUSH_REG_NASM(d_array_code, I_RDX);
    }

    else if (tmp_data->expression_type == FUNCTION) {

        if (!Compile_user_function_nasm(d_array_code, root, func_list, index))
            return false;
    }

    else {

        DEBUG_PRINTF("ERROR: unknown operator\n");
        return false;
    }

    return true;
}

bool Compile_user_function_nasm(Dynamic_array* d_array_code, Node* root,
                                Func_data_list* func_list, int64_t index) {

    int symbols_written = 0;
    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    Variable_data* var_data = NULL;
    memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));

    if (!strncmp(var_data->var_name, "Out", var_data->var_len)) {

        if (!Compile_Out_function_nasm(d_array_code, root, func_list, index))
            return false;
    }

    else if (!strncmp(var_data->var_name, "In", var_data->var_len)) {

        if (!Compile_In_function_nasm(d_array_code, root, func_list, index))
            return false;
    }

    else {

        int64_t callee_index = Find_func_data(func_list, &var_data);
        if (callee_index == -1) {

            DEBUG_PRINTF("ERROR: '%.*s' function was not found\n", (size_t) var_data->var_len, var_data->var_name);
            return false;
        }

        Variable_data check_alignment = {};
        int64_t parameters = (func_list->func_data[callee_index].parameters.free_var);
        if (!(parameters % 2)) {

            check_alignment.var_name = strdup(Check_alignment_even);
            check_alignment.var_len = sizeof(Check_alignment_even) - 1;
        }

        else {

            check_alignment.var_name = strdup(Check_alignment_odd);
            check_alignment.var_len = sizeof(Check_alignment_odd) - 1;
        }

        int parameters_pushed = 0;
        CALL_FUNC_NASM(d_array_code, check_alignment.var_name, check_alignment.var_len);
        free(check_alignment.var_name);
        SUB_REG_REG_NASM(d_array_code, I_RSP, I_RBX);

        Compile_push_parameters_nasm(d_array_code, root->left_node, func_list, index, &parameters_pushed);

        CALL_FUNC_NASM(d_array_code, func_list->func_data[callee_index].function->var_name,
                  func_list->func_data[callee_index].function->var_len);
        ADD_REG_REG_NASM(d_array_code, I_RSP, I_RBX);

        if (Value_size*(parameters-1))
            ADD_REG_IMM_NASM(d_array_code, I_RSP, Value_size*(parameters-1)); // need -1 to push into stack returning value

        int32_t disp32 = 0;
        MOVSD_MEM_XREG_DISP0_NASM(d_array_code, I_RSP, I_XMM0);
    }

    return true;
}

bool Compile_Out_function_nasm(Dynamic_array* d_array_code, Node* root,
                                Func_data_list* func_list, int64_t index) {

    int32_t disp32 = 0, imm32 = 56;
    int parameters_pushed = 0;
    int symbols_written = 0;
    Compile_push_parameters_nasm(d_array_code, root->left_node, func_list, index, &parameters_pushed);
    if (parameters_pushed != 1) {

        DEBUG_PRINTF("ERROR: in func 'Out' wrong arguments amount\n");
        return false;
    }
    MOVSD_XREG_MEM_DISP0_NASM(d_array_code, I_XMM1, I_RSP);
    MOV_REG_MEM_DISP0_NASM(d_array_code, I_RDX, I_RSP);
    SUB_REG_IMM_NASM(d_array_code, I_RSP, imm32); // need to allocate 64 bytes = 32 (shadow space) + 32 (space for xmm0-xmm3),
                                                // but 8 bytes already pushed as a parameter, so 56 bytes
    //disp32 = 0x18;
    //MOVSD_MEM_XREG_DISP0(d_array_code, I_RSP, I_XMM0, ); // windows calling conventions for debug
    int64_t imm64 = 1;
    MOV_REG_IMM64_NASM(d_array_code, I_RAX, imm64);
    LEA_REG_RIP_REL_NASM(d_array_code, I_RCX, FORMAT_OUT_STRING);
    CALL_FUNC_NASM(d_array_code, "printf", sizeof("printf") - 1);

    imm32 = 64;
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);

    return true;
}

bool Compile_In_function_nasm(Dynamic_array* d_array_code, Node* root,
                              Func_data_list* func_list, int64_t index) {

    int32_t disp32 = 0, imm32 = 64;
    int symbols_written = 0;
    SUB_REG_IMM_NASM(d_array_code, I_RSP, imm32);
    LEA_REG_RIP_REL_NASM(d_array_code, I_RCX, FORMAT_IN_STRING);

    /*********Get parameter***********************************/
    if (!root->left_node) {

        DEBUG_PRINTF("ERROR: in func 'In' wrong arguments amount\n");
        return false;
    }

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->left_node->node_data, sizeof(Node_data*));
    Variable_data* var_data = NULL;
    memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
    int64_t var_index = Find_variable(&func_list->func_data[index].local_vars,
                                    var_data->var_name, var_data->var_len);

    if (var_index == -1) {

        var_index = Find_variable(&func_list->func_data[index].parameters,
                                var_data->var_name, var_data->var_len);
        if (var_index == -1) {

            DEBUG_PRINTF("ERROR: variable '%.*s' was not found\n", var_data->var_len, var_data->var_name);
            return false;
        }

        disp32 = (var_index + 2)* 8;    // +1 due to return address & RBP in stack
        LEA_REG_MEM_NASM(d_array_code, I_RDX, I_RBP, disp32);
    }

    else {

        disp32 = -((var_index + 1)* 8);    // +1 due to return address in stack
        LEA_REG_MEM_NASM(d_array_code, I_RDX, I_RBP, disp32);
    }
    /************************************************************** */

    CALL_FUNC_NASM(d_array_code, "scanf", sizeof("scanf") - 1);
    ADD_REG_IMM_NASM(d_array_code, I_RSP, imm32);

    return true;
}

bool Compile_push_parameters_nasm(Dynamic_array* d_array_code, Node* root, Func_data_list* func_list,
                                 int64_t index, int* parameters_pushed) {

    Node_data* tmp_data = NULL;
    memcpy(&tmp_data, &root->node_data, sizeof(Node_data*));
    if (tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

        if (root->left_node)
            if (!Compile_push_parameters_nasm(d_array_code, root->left_node, func_list, index, parameters_pushed))
                return false;

        if (root->right_node)
            if (!Compile_push_parameters_nasm(d_array_code, root->right_node, func_list, index, parameters_pushed))
                return false;
    }

    else {

        if (!Compile_operator_nasm(d_array_code, root, func_list, index))
            return false;

        *parameters_pushed += 1;
    }

    return true;
}
