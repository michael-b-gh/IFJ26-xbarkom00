/**
 * Project: IFJ26 Compiler
 *
 * @file expression.h
 * @brief Precedence syntax analysis for expressions
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#ifndef _EXPRESSION_IFJ26_H
#define _EXPRESSION_IFJ26_H

#include "scanner.h"
#include "symtable.h"

// Indexes for precedence table
typedef enum {
  IDX_MUL_DIV,
  IDX_ADD_SUB,
  IDX_REL,
  IDX_EQ,
  IDX_L_PAR,
  IDX_R_PAR,
  IDX_OPD,
  IDX_END
} prec_idx_t;

// Signs for precedence table
typedef enum { PREC_SH, PREC_RE, PREC_EQ, PREC_ERR } prec_sign_t;

// Content of node in stack
typedef enum { ITEM_MARKER, ITEM_TOKEN, ITEM_NT_E } stack_item_type_t;

// Node in stack
typedef struct stack_item {
  stack_item_type_t type;
  token_t token_type;
  prec_idx_t idx;
  DType data_type;
  struct stack_item *next;
} stack_item_t;

int parse_expression(token *current_token);

#endif
