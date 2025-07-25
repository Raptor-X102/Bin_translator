#ifndef LANG_READ_FUNCS
#define LANG_READ_FUNCS

#include "Lang_funcs.h"

struct Lexeme_data {

    Node* root;
    int64_t line;
    int64_t col;
};

struct Lexeme_array {

    Lexeme_data* lex_arr;
    int64_t size;
    int64_t capacity;
};

const int Mem_to_check = 2;
const int Lex_arr_expansion_coeff = 2;

#define INSERT_PARAMETER var_data = NULL;\
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));\
        index = Insert_var(&func_list->func_data[func_list->free_element].parameters,\
                                   var_data->var_name, var_data->var_value, var_data->var_len);\
        func_list->func_data[func_list->free_element].parameters.var_list[index].RAM_index =\
                                                                        func_list->vars_total;\
        func_list->func_data[func_list->free_element].func_memory++;\
        func_list->vars_total++;

#define INSERT_LOC_VAR \
        Variable_data* var_data = NULL;\
        memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));\
        int64_t index = Find_variable(&func_list->func_data[func_list->free_element].parameters,\
                              var_data->var_name, var_data->var_len);\
        if (index == -1) {\
            index = Find_variable(&func_list->func_data[func_list->free_element].local_vars,\
                                   var_data->var_name, var_data->var_len);\
            if (index == -1) {\
                index = Insert_var(&func_list->func_data[func_list->free_element].local_vars,\
                                        var_data->var_name, var_data->var_value, var_data->var_len);\
                func_list->func_data[func_list->free_element].local_vars.var_list[index].RAM_index =\
                                                                                func_list->vars_total;\
                func_list->func_data[func_list->free_element].func_memory++;\
                func_list->vars_total++;\
            }\
        }

Node* Lang_read_expression(const char* filename, Var_list* func_name_list,
						   Buffer_data* expression_buffer, Func_data_list* func_list,
						   Lexeme_array** lexeme_arr);
Buffer_data Lang_read_file(const char* filename);

Lexeme_array* Lexeme_separator(char* expr_buffer, int64_t expr_buffer_size, Var_list* func_name_list);
bool Lexeme_array_dtor(Lexeme_array* lexeme_array);
bool Lexeme_array_push_back(Lexeme_array* lexeme_array, Node* node, int64_t line, int64_t col);
Node* Get_Grammar(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_if(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset, Func_data_list* func_list);
Node* Get_while(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
                Func_data_list* func_list);
Node* Get_condition(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_comparison(Node_data* node_data1, Node_data* node_data2, int64_t* curr_lex);
Node* Get_user_function_def(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
							Func_data_list* func_list);
Node* Get_user_function(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_def_parameters(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_parameters(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_operation(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
                    Func_data_list* func_list);
Node* Get_assignment(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
bool Exit_operator(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset);
void Skip_lexemes(Lexeme_array* lexeme_array, int64_t* curr_lex);
Node* Get_addition_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_multiplication_priority(Lexeme_array* lexeme_array, int64_t* curr_lex,
                                  Func_data_list* func_list);
Node* Get_power_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_first_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
Node* Get_number(Lexeme_array* lexeme_array, int64_t* curr_lex);
Node* Get_function(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list);
void Syntax_error(Lexeme_array* lexeme_array, int64_t* curr_lex, const char* func, int line);
#define SYNTAX_ERROR(lexeme_array, curr_lex) Syntax_error(lexeme_array, curr_lex, __func__, __LINE__)
bool Lang_get_next_symbol(const char* node_data, uint64_t data_size,
                          int64_t* curr_pos, int64_t* line, int64_t* col);

#endif
