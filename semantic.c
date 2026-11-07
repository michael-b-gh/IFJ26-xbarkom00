/**
 * Project: IFJ26 Compiler
 *
 * @file semantic.c
 * @brief Type checking of expressions
 * @author Lukas Zelenak (xzelenl00)
 */

#include "semantic.h"
#include "error.h"
#include <stdbool.h>

DType check_binary_op(DType leftType, DType rightType, token_t op) {
  switch (op) {
    // 2 Ints = INT, 2 Numbers = DOUBLE, 2 Strings = STRING
  case TOKEN_PLUS:
    if (leftType == DTYPE_INT && rightType == DTYPE_INT)
      return DTYPE_INT;
    if ((leftType == DTYPE_INT || leftType == DTYPE_DOUBLE) &&
        (rightType == DTYPE_INT || rightType == DTYPE_DOUBLE))
      return DTYPE_DOUBLE;
    if (leftType == DTYPE_STRING && rightType == DTYPE_STRING)
      return DTYPE_STRING;
    return DTYPE_UNKNOWN;

    // 2 Ints = INT, 2 Numbers = DOUBLE
  case TOKEN_MINUS:
  case TOKEN_MUL:
  case TOKEN_DIV:
    if (leftType == DTYPE_INT && rightType == DTYPE_INT)
      return DTYPE_INT;
    if ((leftType == DTYPE_INT || leftType == DTYPE_DOUBLE) &&
        (rightType == DTYPE_INT || rightType == DTYPE_DOUBLE))
      return DTYPE_DOUBLE;
    return DTYPE_UNKNOWN;

    // 2 Numbers or 2 Strings = BOOLEAN
  case TOKEN_LESS_THAN:
  case TOKEN_LESS_THAN_EQ:
  case TOKEN_MORE_THAN:
  case TOKEN_MORE_THAN_EQ:
    if (((leftType == DTYPE_INT || leftType == DTYPE_DOUBLE) &&
         (rightType == DTYPE_INT || rightType == DTYPE_DOUBLE)) ||
        (leftType == DTYPE_STRING && rightType == DTYPE_STRING))
      return DTYPE_BOOLEAN;
    return DTYPE_UNKNOWN;

    // 2 Numbers or 2 Same Types = BOOLEAN
  case TOKEN_EQ:
  case TOKEN_NOT_EQ:
    if (((leftType == DTYPE_INT || leftType == DTYPE_DOUBLE) &&
         (rightType == DTYPE_INT || rightType == DTYPE_DOUBLE)) ||
        (leftType == rightType && leftType != DTYPE_UNKNOWN &&
         leftType != DTYPE_BOOLEAN))
      return DTYPE_BOOLEAN;
    return DTYPE_UNKNOWN;

  default:
    return DTYPE_UNKNOWN;
  }
}

DType check_unary_op(DType operandType, token_t op) {
  switch (op) {
    // INT = INT, DOUBLE = DOUBLE
  case TOKEN_PLUS:
  case TOKEN_MINUS:
    if (operandType == DTYPE_INT || operandType == DTYPE_DOUBLE)
      return operandType;
    return DTYPE_UNKNOWN;

  default:
    return DTYPE_UNKNOWN;
  }
}

DType get_term_type(token *tok, SymTable *symtable, int *err_code) {
  *err_code = SUCCESS;
  switch (tok->token_type) {
  case TOKEN_LIT_INT:
    return DTYPE_INT;
  case TOKEN_LIT_DOUBLE:
    return DTYPE_DOUBLE;
  case TOKEN_LIT_STRING:
    return DTYPE_STRING;
  case TOKEN_UNIT:
    return DTYPE_UNIT;
  case TOKEN_ID: {
    Symbol *symbol = symTableSearch(symtable, tok->attribute);
    if (symbol == NULL || symbol->kind != SYM_VARIABLE) {
      *err_code = UNDEF_SEM_ERR;
      return DTYPE_UNKNOWN;
    }
    return symbol->type;
  }
  default:
    return DTYPE_UNKNOWN;
  }
}

int check_assignment(SymTable *symtable, const char *name) {
  Symbol *symbol = symTableSearch(symtable, name);
  if (symbol == NULL || symbol->kind == SYM_FUNCTION)
    return UNDEF_SEM_ERR;
  if (symbol->isMutable == false)
    return VAR_SEM_ERR;

  return SUCCESS;
}

int check_main_function(size_t paramCount, DType returnType) {
  if (paramCount != 0)
    return PARAM_SEM_ERR;
  if (returnType != DTYPE_UNIT)
    return TYPE_INC_SEM_ERR;
  return SUCCESS;
}