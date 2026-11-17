/**
 * Project: IFJ26 Compiler
 *
 * @file parser.c
 * @brief Parser verifies that the string of tokens represents a syntactically
 * well-formed program
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#include "error.h"
#include "expression.h"
#include "scanner.h"
#include "semantic.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_t peek_type(const TokenBuffer *buf, size_t *idx);
int parse_start(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_prolog(const TokenBuffer *buf, size_t *idx);
int parse_main(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_variable(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_function(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_if(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_while(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_type(const TokenBuffer *buf, size_t *idx, DType *data_type);
int parse_param(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_block(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_return(const TokenBuffer *buf, size_t *idx, SymTable *st);
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, SymTable *st,
                       DType *data_type);
int parse_func_call(const TokenBuffer *buf, size_t *idx, SymTable *st);

/**
 * @brief Peeks at the type of next token
 *
 * @param buf Buffer to read from
 * @param idx Index of the current token
 *
 * @return Token type
 */
token_t peek_type(const TokenBuffer *buf, size_t *idx) {
  token *next_tok = tokenBufferGet(buf, *idx + 1);

  if (next_tok == NULL)
    return TOKEN_EOF;
  else
    return next_tok->token_type;
}

/**
 * @brief Top-level driver for recursive descent parsing
 *
 * @param buf Buffer to read from
 * @param idx Index of the current token
 * @param st
 *
 * @return Return code (SUCCESS/Type of error)
 */
int parse_start(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  int res;
  res = parse_prolog(buf, idx);
  if (res != SUCCESS)
    return res;

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOF) {
    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
      (*idx)++;

    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_DEF) {
      res = parse_function(buf, idx, st);
      if (res != SUCCESS)
        return res;
    }

    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_MAIN) {
      res = parse_main(buf, idx, st);
      if (res != SUCCESS)
        return res;
    } else
      return SYN_ERR;
  }

  return SUCCESS;
}

// Verifies grammatical correctness of the prolog
int parse_prolog(const TokenBuffer *buf, size_t *idx) {
  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_IMPORT)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ID ||
      tokenBufferGet(buf, *idx)->attribute == NULL ||
      strcmp(tokenBufferGet(buf, *idx)->attribute, "ifj26") != 0)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DOT)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_MUL)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

// Verifies grammatical correctness of the main
int parse_main(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  (*idx)++;

  while (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
    (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DEF)
    return SYN_ERR;

  return parse_function(buf, idx, st);
}

// Verifies grammatical correctness of the variable definition
int parse_variable(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  int res;
  bool mut = (tokenBufferGet(buf, *idx)->token_type == TOKEN_VAR);
  DType init_type = DTYPE_UNKNOWN;
  DType annot_type = DTYPE_UNKNOWN;

  (*idx)++;

  char *name = tokenBufferGet(buf, *idx)->attribute;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ID)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COLON) {
    (*idx)++;

    res = parse_type(buf, idx, &annot_type);
    if (res != SUCCESS)
      return res;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ASSIGN)
    return SYN_ERR;

  (*idx)++;

  res = parse_expr_or_call(buf, idx, st, &init_type);

  if (res != SUCCESS)
    return res;

  if (init_type == DTYPE_BOOLEAN)
    return TYPE_INC_SEM_ERR;

  res = check_variable_def(st, name, annot_type, init_type, mut);
  if (res != SUCCESS)
    return res;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

// Verifies grammatical correctness of the function definition
int parse_function(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  int res;
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ID)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_L_PARENTHESIS)
    return SYN_ERR;

  (*idx)++;

  res = parse_param(buf, idx, st);
  if (res != SUCCESS)
    return res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COLON) {
    (*idx)++;

    res = parse_type(buf, idx, NULL);
    if (res != SUCCESS)
      return res;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ASSIGN)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    (*idx)++;

    res = parse_block(buf, idx, st);
    if (res != SUCCESS)
      return res;
  } else {
    res = parse_expr_or_call(buf, idx, st, NULL);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
      return SYN_ERR;

    (*idx)++;
  }

  return SUCCESS;
}

// Verifies grammatical correctness of an if-statement
int parse_if(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  DType cond_type = DTYPE_UNKNOWN;
  int res;
  (*idx)++;

  // Check for condition
  res = parse_expression(buf, idx, st, &cond_type);
  if (res != SUCCESS)
    return res;

  if (cond_type != DTYPE_BOOLEAN)
    return TYPE_INC_SEM_ERR;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_THEN)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, st);
  if (res != SUCCESS)
    return res;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ELSE)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, st);
  if (res != SUCCESS)
    return res;

  return SUCCESS;
}

// Verifies grammatical correctness of a while-loop
int parse_while(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  DType cond_type = DTYPE_UNKNOWN;
  int res;
  (*idx)++;

  // Check for condition
  res = parse_expression(buf, idx, st, &cond_type);
  if (res != SUCCESS)
    return res;

  if (cond_type != DTYPE_BOOLEAN)
    return TYPE_INC_SEM_ERR;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DO)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, st);
  if (res != SUCCESS)
    return res;

  return SUCCESS;
}

// Verifies data type annotations
int parse_type(const TokenBuffer *buf, size_t *idx, DType *data_type) {
  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_INT ||
      tokenBufferGet(buf, *idx)->token_type == TOKEN_DOUBLE ||
      tokenBufferGet(buf, *idx)->token_type == TOKEN_STRING ||
      tokenBufferGet(buf, *idx)->token_type == TOKEN_UNIT) {

    if (data_type != NULL) {
      switch (tokenBufferGet(buf, *idx)->token_type) {
      case TOKEN_INT:
        *data_type = DTYPE_INT;
        break;
      case TOKEN_DOUBLE:
        *data_type = DTYPE_DOUBLE;
        break;
      case TOKEN_STRING:
        *data_type = DTYPE_STRING;
        break;
      case TOKEN_UNIT:
        *data_type = DTYPE_UNIT;
        break;

      default:
        return SYN_ERR;
      }
    }

    (*idx)++;

    return SUCCESS;
  }

  return SYN_ERR;
}

// Verifies function parameter definitions
int parse_param(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  (void)st;
  int res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_R_PARENTHESIS) {
    (*idx)++;
    return SUCCESS;
  }

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_R_PARENTHESIS) {
    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID)
      (*idx)++;
    else
      return SYN_ERR;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COLON)
      (*idx)++;
    else
      return SYN_ERR;

    res = parse_type(buf, idx, NULL);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COMMA)
      (*idx)++;
    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_R_PARENTHESIS) {
      // Parser hit ")": termination of the loop
    } else
      return SYN_ERR;
  }
  (*idx)++;
  return SUCCESS;
}

// Verifies block statements and literals
int parse_block(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  int res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IND) {

    symTablePushScope(st);

    (*idx)++;

    while (tokenBufferGet(buf, *idx)->token_type != TOKEN_DED) {
      if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
        (*idx)++;

      else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_VAR ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_VAL) {
        res = parse_variable(buf, idx, st);
        if (res != SUCCESS)
          return res;

      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IF) {
        res = parse_if(buf, idx, st);
        if (res != SUCCESS)
          return res;

      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_WHILE) {
        res = parse_while(buf, idx, st);
        if (res != SUCCESS)
          return res;

      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_RETURN) {
        res = parse_return(buf, idx, st);
        if (res != SUCCESS)
          return res;

      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
                 peek_type(buf, idx) == TOKEN_ASSIGN) {
        char *name = tokenBufferGet(buf, *idx)->attribute;
        DType rhs_type = DTYPE_UNKNOWN;
        (*idx)++;
        (*idx)++;

        res = parse_expr_or_call(buf, idx, st, &rhs_type);
        if (res != SUCCESS)
          return res;

        if (rhs_type == DTYPE_BOOLEAN)
          return TYPE_INC_SEM_ERR;

        res = check_assignment(st, name, rhs_type);
        if (res != SUCCESS)
          return res;

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        (*idx)++;
      } else {
        res = parse_expr_or_call(buf, idx, st, NULL);
        if (res != SUCCESS)
          return res;

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        (*idx)++;
      }
    }
    symTablePopScope(st);
  } else
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

// Verifies return
int parse_return(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    (*idx)++;
    return SUCCESS;
  } else {
    int res = parse_expr_or_call(buf, idx, st, NULL);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
      return SYN_ERR;

    (*idx)++;

    return SUCCESS;
  }
}

// Parses an expression or a function call
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, SymTable *st,
                       DType *data_type) {
  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
      peek_type(buf, idx) == TOKEN_L_PARENTHESIS) {
    if (data_type != NULL)
      *data_type = DTYPE_UNIT;

    return parse_func_call(buf, idx, st);
  } else
    return parse_expression(buf, idx, st, data_type);
}

// Verifies function call
int parse_func_call(const TokenBuffer *buf, size_t *idx, SymTable *st) {
  (void)st;
  (*idx)++;
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_R_PARENTHESIS) {
    (*idx)++;
    return SUCCESS;
  }

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_R_PARENTHESIS) {
    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ID &&
        tokenBufferGet(buf, *idx)->token_type != TOKEN_LIT_INT &&
        tokenBufferGet(buf, *idx)->token_type != TOKEN_LIT_DOUBLE &&
        tokenBufferGet(buf, *idx)->token_type != TOKEN_LIT_STRING &&
        tokenBufferGet(buf, *idx)->token_type != TOKEN_UNIT)
      return SYN_ERR;

    (*idx)++;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COMMA)
      (*idx)++;
    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_R_PARENTHESIS) {
      // Parser hit ")": termination of the loop
    } else
      return SYN_ERR;
  }

  (*idx)++;
  return SUCCESS;
}

// @brief Main entry point; initializes the token stream and begins
// recursive descent parsing
int main() {
  TokenBuffer *buf = tokenBufferCreate();

  int res;

  res = tokenBufferLoad(buf);
  if (res != SUCCESS) {
    tokenBufferFree(buf);
    return res;
  }

  size_t idx = 0;

  SymTable *st = symTableCreate();

  res = parse_start(buf, &idx, st);
  tokenBufferFree(buf);
  symTableFree(st);

  return res;
}
