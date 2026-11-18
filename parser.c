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

#define NAMES 1000
#define TYPES 1000

typedef struct ParserCtx {
  SymTable *st;
  int pass;
  int first_sem_err;
  Symbol *curr_func;
} ParserCtx;

token_t peek_type(const TokenBuffer *buf, size_t *idx);
void record_sem_err(ParserCtx *ctx, int err);
int parse_start(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_prolog(const TokenBuffer *buf, size_t *idx);
int parse_main(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_variable(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_function(const TokenBuffer *buf, size_t *idx, bool is_main,
                   ParserCtx *ctx);
int parse_if(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_while(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_type(const TokenBuffer *buf, size_t *idx, DType *data_type);
int parse_param(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                const char **param_names, DType *param_types,
                size_t *param_count);
int parse_block(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_return(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                       DType *data_type);
int parse_func_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);

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

void record_sem_err(ParserCtx *ctx, int err) {
  if (err != SUCCESS && ctx->first_sem_err == SUCCESS)
    ctx->first_sem_err = err;
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
int parse_start(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  int res;

  res = parse_prolog(buf, idx);
  if (res != SUCCESS)
    return res;

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOF) {
    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
      (*idx)++;

    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_DEF) {
      res = parse_function(buf, idx, false, ctx);
      if (res != SUCCESS)
        return res;
    }

    else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_MAIN) {
      res = parse_main(buf, idx, ctx);
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
int parse_main(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  (*idx)++;

  while (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
    (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DEF)
    return SYN_ERR;

  return parse_function(buf, idx, true, ctx);
}

// Verifies grammatical correctness of the variable definition
int parse_variable(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
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

  res = parse_expr_or_call(buf, idx, ctx, &init_type);

  if (res != SUCCESS)
    return res;

  if (ctx->pass == 2) {
    if (init_type == DTYPE_BOOLEAN)
      return TYPE_INC_SEM_ERR;

    res = check_variable_def(ctx->st, name, annot_type, init_type, mut);
    if (res != SUCCESS)
      return res;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

// Verifies grammatical correctness of the function definition
int parse_function(const TokenBuffer *buf, size_t *idx, bool is_main,
                   ParserCtx *ctx) {
  const char *param_names[NAMES];
  DType param_types[TYPES];
  size_t param_count = 0;

  int res;
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ID)
    return SYN_ERR;

  const char *func_name = tokenBufferGet(buf, *idx)->attribute;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_L_PARENTHESIS)
    return SYN_ERR;

  (*idx)++;

  res = parse_param(buf, idx, ctx, param_names, param_types, &param_count);
  if (res != SUCCESS)
    return res;

  bool has_explicit_ret = false;
  DType ret_type = DTYPE_UNKNOWN;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COLON) {
    (*idx)++;
    has_explicit_ret = true;

    res = parse_type(buf, idx, &ret_type);
    if (res != SUCCESS)
      return res;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ASSIGN)
    return SYN_ERR;

  (*idx)++;

  size_t body_start_idx = *idx;

  if (ctx->pass == 1) {
    if (is_main) {
      DType main_ret = DTYPE_UNIT;
      if (has_explicit_ret)
        main_ret = ret_type;

      int err = check_main_function(param_count, main_ret);
      record_sem_err(ctx, err);
    }
    Symbol *symtable = symTableDefineFunction(ctx->st, func_name, param_count,
                                              param_types, ret_type, is_main,
                                              has_explicit_ret, body_start_idx);

    if (symtable == NULL)
      record_sem_err(ctx, REDEF_SEM_ERR);
  }

  if (ctx->pass == 2) {
    ctx->curr_func = symTableSearch(ctx->st, func_name);
    symTablePushScope(ctx->st);

    for (size_t i = 0; i < param_count; i++)
      symTableInsertVariable(ctx->st, param_names[i], param_types[i], false);
  }

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    (*idx)++;

    res = parse_block(buf, idx, ctx);
    if (res != SUCCESS)
      return res;
  } else {
    res = parse_expr_or_call(buf, idx, ctx, NULL);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
      return SYN_ERR;

    (*idx)++;
  }

  if (ctx->pass == 2)
    symTablePopScope(ctx->st);

  return SUCCESS;
}

// Verifies grammatical correctness of an if-statement
int parse_if(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  DType cond_type = DTYPE_UNKNOWN;
  int res;
  (*idx)++;

  SymTable *expr_st = NULL;
  if (ctx->pass == 2)
    expr_st = ctx->st;

  // Check for condition in 2nd pass
  res = parse_expression(buf, idx, expr_st, &cond_type);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 2) {
    if (cond_type != DTYPE_BOOLEAN)
      return TYPE_INC_SEM_ERR;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_THEN)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx);
  if (res != SUCCESS)
    return res;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ELSE)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx);
  if (res != SUCCESS)
    return res;

  return SUCCESS;
}

// Verifies grammatical correctness of a while-loop
int parse_while(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  DType cond_type = DTYPE_UNKNOWN;
  int res;
  (*idx)++;

  SymTable *expr_st = NULL;
  if (ctx->pass == 2)
    expr_st = ctx->st;

  // Check for condition in 2nd pass
  res = parse_expression(buf, idx, expr_st, &cond_type);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 2) {
    if (cond_type != DTYPE_BOOLEAN)
      return TYPE_INC_SEM_ERR;
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DO)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx);
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
int parse_param(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                const char **param_names, DType *param_types,
                size_t *param_count) {
  int res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_R_PARENTHESIS) {
    (*idx)++;
    return SUCCESS;
  }

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_R_PARENTHESIS) {
    const char *param_name = NULL;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID) {
      param_name = tokenBufferGet(buf, *idx)->attribute;
      (*idx)++;
    } else
      return SYN_ERR;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COLON)
      (*idx)++;
    else
      return SYN_ERR;

    DType param_type = DTYPE_UNKNOWN;

    res = parse_type(buf, idx, &param_type);
    if (res != SUCCESS)
      return res;

    if (ctx->pass == 1) {
      for (size_t i = 0; i < *param_count; i++)
        if (strcmp(param_names[i], param_name) == 0)
          record_sem_err(ctx, REDEF_SEM_ERR);
    }

    param_names[*param_count] = param_name;
    param_types[*param_count] = param_type;
    (*param_count)++;

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
int parse_block(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  int res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IND) {

    if (ctx->pass == 2)
      symTablePushScope(ctx->st);

    (*idx)++;

    while (tokenBufferGet(buf, *idx)->token_type != TOKEN_DED) {
      if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
        (*idx)++;

      else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_VAR ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_VAL) {
        res = parse_variable(buf, idx, ctx);
        if (res != SUCCESS)
          return res;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IF) {
        res = parse_if(buf, idx, ctx);
        if (res != SUCCESS)
          return res;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_WHILE) {
        res = parse_while(buf, idx, ctx);
        if (res != SUCCESS)
          return res;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_RETURN) {
        res = parse_return(buf, idx, ctx);
        if (res != SUCCESS)
          return res;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
                 peek_type(buf, idx) == TOKEN_ASSIGN) {
        char *name = tokenBufferGet(buf, *idx)->attribute;
        DType rhs_type = DTYPE_UNKNOWN;
        (*idx)++;
        (*idx)++;

        res = parse_expr_or_call(buf, idx, ctx, &rhs_type);
        if (res != SUCCESS)
          return res;

        if (ctx->pass == 2) {
          if (rhs_type == DTYPE_BOOLEAN)
            return TYPE_INC_SEM_ERR;

          res = check_assignment(ctx->st, name, rhs_type);
          if (res != SUCCESS)
            return res;
        }

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        (*idx)++;
      } else {
        res = parse_expr_or_call(buf, idx, ctx, NULL);
        if (res != SUCCESS)
          return res;

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        (*idx)++;
      }
    }
    if (ctx->pass == 2) {
      symTablePopScope(ctx->st);
    }
  } else
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

// Verifies return
int parse_return(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    (*idx)++;
    return SUCCESS;
  } else {
    int res = parse_expr_or_call(buf, idx, ctx, NULL);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
      return SYN_ERR;

    (*idx)++;

    return SUCCESS;
  }
}

// Parses an expression or a function call
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                       DType *data_type) {
  SymTable *expr_st = NULL;
  if (ctx->pass == 2)
    expr_st = ctx->st;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
      peek_type(buf, idx) == TOKEN_L_PARENTHESIS) {
    if (data_type != NULL)
      *data_type = DTYPE_UNIT;

    return parse_func_call(buf, idx, ctx);
  } else
    return parse_expression(buf, idx, expr_st, data_type);
}

// Verifies function call
int parse_func_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  (void)ctx;
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

  ParserCtx ctx = {0};
  ctx.st = symTableCreate();

  ctx.pass = 1;
  ctx.first_sem_err = SUCCESS;
  size_t idx = 0;

  res = parse_start(buf, &idx, &ctx);
  if (res != SUCCESS) {
    tokenBufferFree(buf);
    symTableFree(ctx.st);
    return res;
  }

  if (ctx.first_sem_err != SUCCESS) {
    tokenBufferFree(buf);
    symTableFree(ctx.st);
    return ctx.first_sem_err;
  }

  res = check_main_exists(ctx.st);
  if (res != SUCCESS) {
    tokenBufferFree(buf);
    symTableFree(ctx.st);
    return res;
  }

  ctx.pass = 2;
  idx = 0;
  res = parse_start(buf, &idx, &ctx);

  tokenBufferFree(buf);
  symTableFree(ctx.st);

  return res;
}
