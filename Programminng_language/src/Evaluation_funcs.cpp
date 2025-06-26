#include "Evaluation_funcs.h"

Node* DEBUG_NODE = NULL;

Node* Evaluate_const_node(Node* root) {

    Node_data* tmp_root_data = NULL, * tmp_node_data1 = NULL, * tmp_node_data2 = NULL;
    if(root)
        memcpy(&tmp_root_data, &root->node_data, sizeof(Node_data*));
    if(root->left_node)
        memcpy(&tmp_node_data1, &root->left_node->node_data, sizeof(Node_data*));
    if(root->right_node)
        memcpy(&tmp_node_data2, &root->right_node->node_data, sizeof(Node_data*));

    if(tmp_root_data->expression_type == OP) {

        if((tmp_root_data->value & Is_binary) == Is_binary) {

            if(tmp_node_data1->expression_type == NUM && tmp_node_data2->expression_type == NUM) {

                double tmp_double1 = 0, tmp_double2 = 0;
                memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));
                memcpy(&tmp_double2, &tmp_node_data2->value, sizeof(double));
                double tmp_double_res = Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2);
                return _NUM(tmp_double_res);
            }

            else
                return NULL;
        }

        else {

            if(tmp_node_data1->expression_type == NUM) {

                double tmp_double1 = 0, tmp_double2_zero = 0;
                memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));
                double tmp_double_res = Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2_zero);
                return _NUM(tmp_double_res);
            }

            else
                return NULL;
        }
    }

    else
        return NULL;
}

double Evaluate_tree(Var_list* var_list, Node* root) {

    Node_data* tmp_root_data = NULL, * tmp_node_data1 = NULL, * tmp_node_data2 = NULL;
    if(root) {

        memcpy(&tmp_root_data, &root->node_data, sizeof(Node_data*));
        Print_node_data(&tmp_root_data, stderr);
    }

    if(root->left_node) {

        memcpy(&tmp_node_data1, &root->left_node->node_data, sizeof(Node_data*));
        Print_node_data(&tmp_node_data1, stderr);
    }

    if(root->right_node) {

        memcpy(&tmp_node_data2, &root->right_node->node_data, sizeof(Node_data*));
        Print_node_data(&tmp_node_data2, stderr);
    }

    double tmp_double1 = 0, tmp_double2 = 0;

    if(tmp_node_data1->expression_type == OP)
        tmp_double1 = Evaluate_tree(var_list, root->left_node);


    if(tmp_node_data1->expression_type == VAR) {

        Variable_data* vl_ptr = NULL;
        memcpy(&vl_ptr, &tmp_node_data1->value, sizeof(Variable_data*));

        int64_t index = Find_variable(var_list, vl_ptr->var_name, vl_ptr->var_len);
        if(index < 0) {

            DEBUG_PRINTF("ERROR: no such variable in var_list");
            return Error_val;
        }

        tmp_double1 = var_list->var_list[index].var_value;
    }

    if(tmp_node_data1->expression_type == NUM)
        memcpy(&tmp_double1, &tmp_node_data1->value, sizeof(double));

    if((tmp_root_data->value & Is_binary) == Is_binary) {

        if(tmp_node_data2->expression_type == OP) {

            tmp_double2 = Evaluate_tree(var_list, root->right_node);
            //TREE_CREATION(1, tmp_double2);
        }

        if(tmp_node_data2->expression_type == VAR) {

            Variable_data* vl_ptr = NULL;
            memcpy(&vl_ptr, &tmp_node_data2->value, sizeof(Variable_data*));

            int64_t index = Find_variable(var_list, vl_ptr->var_name, vl_ptr->var_len);
            if(index < 0) {

                DEBUG_PRINTF("ERROR: no such variable in var_list");
                return 0;
            }

            tmp_double2 = var_list->var_list[index].var_value;
        }

        if(tmp_node_data2->expression_type == NUM)
            memcpy(&tmp_double2, &tmp_node_data2->value, sizeof(double));
    }

    DEBUG_NODE = root;
    return Do_arithmetics(tmp_root_data->value, tmp_double1, tmp_double2);
}

double Do_arithmetics(int64_t operation, double num1, double num2) {

    switch (operation){

        case ADD:
            return num1 + num2;

        case SUB:
            return num1 - num2;

        case MUL:
            return num1 * num2;

        case DIV:

            if(fabs(num2) > Epsilon)
                return num1 / num2;

            else {

                DEBUG_PRINTF("ERROR: zero division\n");
                return 0;
            }

        case POW:

            if(fabs(num1) < Epsilon && fabs(num2) < Epsilon) {
                DEBUG_PRINTF("ERROR: 0 ^ 0\n");
                DEBUG_PRINTF("num1 = %lf\n", num1);
                DEBUG_PRINTF("num2 = %lf\n", num2);
                Tree_graph_debug(DEBUG_NODE, "DEBUG.dot", &Print_node_data);
                abort();
                return 0;
            }

            else
                return pow(num1, num2);

        case LN:

            if(fabs(num1) > Epsilon)
                return log(num1);

            else {

                DEBUG_PRINTF("ERROR: ln(0)\n");
                return 0;
            }

        case SH:
            return sinh(num1);

        case CH:
            return cosh(num1);

        case TH:
            return tanh(num1);

        case LOG:

            if(fabs(num1) < Epsilon && fabs(num2) < Epsilon || fabs((num1 - 1)) < Epsilon) {

                DEBUG_PRINTF("ERROR: wrong log expression\n");
                return 0;
            }

            else
                return log(num1) / log(num2);

        case SIN:
            return sin(num1);

        case COS:
            return cos(num1);

        case TAN:

            if(fabs(fmod(num2, M_PI / 2)) > Epsilon)
                return tan(num1);

            else {

                DEBUG_PRINTF("ERROR: tan((Pi/2) + (Pi * n))\n");
                return 0;
            }

        case COT:

            if(fabs(fmod(num2, M_PI)) > Epsilon)
                return tan(M_PI / 2 - num1);

            else {

                DEBUG_PRINTF("ERROR: cot(Pi + (Pi * n))\n");
                return 0;
            }

        case EXP:
            return exp(num1);

        case CTH:

            if(fabs(num1) > Epsilon)
                return 1 / tanh(num1);

            else {

                DEBUG_PRINTF("ERROR: coth argument mustn't be 0\n");
                return 0;
            }

        case ASH:
            return asinh(num1);

        case ACH:

            if(num1 > 1)
                return acosh(num1);

            else {

                DEBUG_PRINTF("ERROR: arccosh argument must be > 1\n");
                return 0;
            }

        case ATH:

            if(-1 < num1 && num1 < 1)
                return atanh(num1);

            else {

                DEBUG_PRINTF("ERROR: arctanh argument must belong to (-1;1)\n");
                return 0;
            }

        case ABS:
            return fabs(num1);

        case ASIN:

            if((num1 > -1 || num1 + 1 < Epsilon) &&
               (num1 < 1  || 1 - num1 < Epsilon))
                return asin(num1);

            else {

                DEBUG_PRINTF("ERROR: arcsin argument must belong to [-1;1]\n");
                return 0;
            }

        case ACOS:

            if((num1 > -1 || num1 + 1 < Epsilon) &&
               (num1 < 1  || 1 - num1 < Epsilon))
                return acos(num1);

            else {

                DEBUG_PRINTF("ERROR: arccos argument must belong to [-1;1]\n");
                return 0;
            }

        case ATAN:
            return atan(num1);

        case ACOT:
            return M_PI / 2 - atan(num1);

        case ACTH:
            if(-1 < num1 && num1 < 1) {

                DEBUG_PRINTF("ERROR: arccoth argument mustn't belong to [-1;1]\n");
                return 0;
            }

            else
                return 0.5 * log((num1 + 1) / (num1 - 1));

        case SQRT:

            if(num1 > 0 || num1 > Epsilon)
                return sqrt(num1);

            else {

                DEBUG_PRINTF("ERROR: sqrt argument mustn't be negative\n");
                return 0;
            }

        default:
            DEBUG_PRINTF("ERROR: no such function");
    }

    return 0;
}

double Fact(int n) {

    double res = 1;
    for(int i = 2; i <= n; i++)
        res *= i;

    return res;
}
