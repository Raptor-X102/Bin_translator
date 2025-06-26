#ifndef SIMPLIFICATION_FUNCS
#define SIMPLIFICATION_FUNCS

#include "Lang_funcs.h"
#include "Evaluation_funcs.h"

static int64_t Default_line = -0x1;
static int64_t Default_col = -0x1;

bool Simplify_expression(Node** root, Lexeme_array* lexeme_array);
bool Simplify_expression_left(Node** root, int64_t operation, double left_constant,
                              Lexeme_array* lexeme_array);
bool Simplify_expression_right(Node** root, int64_t operation, double right_constant,
                               Lexeme_array* lexeme_array);
Node* Copy_node(Node* root);

#endif
