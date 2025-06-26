#ifndef EVAL_FUNCS
#define EVAL_FUNCS

#include "Lang_funcs.h"

#define TREE_CREATION(tmp_double1, tmp_double2) if(!tmp_double1 || !tmp_double2) {\
        Tree_graph_debug(root, "DEBUG.dot", &Print_node_data);\
        abort();\
    }

const double Error_val = -1;

Node* Evaluate_const_node(Node* root);
double Evaluate_tree(Var_list* var_list, Node* root);
double Do_arithmetics(int64_t operation, double num1, double num2);
double Fact(int n);
void Print_node_data(const void* data, FILE* file);

#endif
