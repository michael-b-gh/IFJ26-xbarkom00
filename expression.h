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
#include "token_buffer.h"

// Indexes for precedence table
typedef enum {
  IDX_MUL_DIV,
  IDX_ADD_SUB,
  IDX_REL,
  IDX_EQ,
  IDX_L_PAR,
  IDX_R_PAR,
  IDX_OPD,
  IDX_UNARY,
  IDX_END,
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

/**
 * @brief Parses, type-checks, and optionally generates IFJcode26 for an
 * expression
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current token index in the token buffer
 * @param symtable Pointer to the symbol table (if NULL, type checking is
 * skipped)
 * @param data_type Output pointer for the resulting expression type (may be
 * NULL)
 * @param emit_code If true, emits IFJcode26 instructions to stdout
 *
 * @return SUCCESS on success, or an error code
 */
int parse_expression(const TokenBuffer *buf, size_t *idx, SymTable *symtable,
                     DType *data_type, bool emit_code);

/**
 * @brief Emits a PUSHS instruction for a literal or variable token
 *
 * @param tok Pointer to the operand token (literal or variable identifier)
 * @param symtable Pointer to the symbol table (used to resolve variable IDs)
 */
void emit_push_term(const token *tok, const SymTable *symtable);

#endif
