/**
 * Project: IFJ26 Compiler
 *
 * @file parser.c
 * @brief Parser verifies that the string of tokens represents a syntactically
 * well-formed program
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#include "codegen.h"
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
  const char *main_name;
} ParserCtx;

token_t peek_type(const TokenBuffer *buf, size_t *idx);
void record_sem_err(ParserCtx *ctx, int err);
int parse_start(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_prolog(const TokenBuffer *buf, size_t *idx);
int parse_main(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_variable(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_function(const TokenBuffer *buf, size_t *idx, bool is_main,
                   ParserCtx *ctx);
int parse_if(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx, bool *ret_out,
             DType *data_type);
int parse_while(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_type(const TokenBuffer *buf, size_t *idx, DType *data_type,
               ParserCtx *ctx);
int parse_param(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                const char **param_names, DType *param_types,
                size_t *param_count);
int parse_block(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                DType *out_last_type, bool *ret_out);
int parse_return(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx);
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                       DType *data_type);
int parse_func_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                    DType *data_type);

/**
 * @brief Peeks at the type of next token
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 *
 * @return Token type of next token
 */
token_t peek_type(const TokenBuffer *buf, size_t *idx) {
  token *next_tok = tokenBufferGet(buf, *idx + 1);

  if (next_tok == NULL)
    return TOKEN_EOF;
  else
    return next_tok->token_type;
}

/**
 * @brief Records semantic error in first pass
 *
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param err code of semantic err?
 */
void record_sem_err(ParserCtx *ctx, int err) {
  if (err != SUCCESS && ctx->first_sem_err == SUCCESS)
    ctx->first_sem_err = err;
}

/**
 * @brief Top-level driver for recursive descent parsing
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
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

/**
 * @brief Verifies grammatical correctness of the prolog
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 *
 * @return SUCCESS on success, or SYN_ERR on syntax error
 */
int parse_prolog(const TokenBuffer *buf, size_t *idx) {
  while (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
    (*idx)++;

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

/**
 * @brief Verifies grammatical correctness of the main
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or SYN_ERR on syntax error
 */
int parse_main(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  (*idx)++;

  while (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
    (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DEF)
    return SYN_ERR;

  return parse_function(buf, idx, true, ctx);
}

/**
 * @brief Verifies grammatical correctness of the variable definition
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
 */
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

    res = parse_type(buf, idx, &annot_type, ctx);
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

  if (ctx->pass == 3) {
    DType fin_type;

    if (annot_type != DTYPE_UNKNOWN)
      fin_type = annot_type;
    else
      fin_type = init_type;

    Symbol *sym = symTableInsertVariable(ctx->st, name, fin_type,
                                         mut); // a co datovy typ?

    if (fin_type == DTYPE_DOUBLE && init_type == DTYPE_INT)
      generate_int2floats();

    printf("POPS LF@v$%zu\n", sym->varId);
  }

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;
  return SUCCESS;
}

/**
 * @brief Verifies grammatical correctness of the function definition and
 * generates code
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param is_main True if the function is annotated with @main
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
 */
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

    res = parse_type(buf, idx, &ret_type, ctx);
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
      ctx->main_name = func_name;
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
    ctx->curr_func->firstVarId = symTableGetNextVarId(ctx->st);
    symTablePushScope(ctx->st);

    for (size_t i = 0; i < param_count; i++)
      symTableInsertVariable(ctx->st, param_names[i], param_types[i], false);
  }

  if (ctx->pass == 3) {
    generate_func_start(func_name);

    ctx->curr_func = symTableSearch(ctx->st, func_name);
    symTableSetNextVarId(ctx->st, ctx->curr_func->firstVarId);

    printf("DEFVAR LF@%%retval\n");
    printf("MOVE LF@%%retval nil@nil\n");

    for (size_t var = ctx->curr_func->firstVarId;
         var < ctx->curr_func->lastVarId; var++)
      printf("DEFVAR LF@v$%zu\n", var);

    symTablePushScope(ctx->st);

    for (size_t i = 0; i < param_count; i++)
      symTableInsertVariable(ctx->st, param_names[i], param_types[i], false);

    for (size_t i = param_count; i > 0; i--) {
      Symbol *param_sym = symTableSearch(ctx->st, param_names[i - 1]);

      printf("POPS LF@v$%zu\n", param_sym->varId);
    }
  }

  DType body_type = DTYPE_UNKNOWN;
  bool ret_all = false;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    (*idx)++;

    res = parse_block(buf, idx, ctx, &body_type, &ret_all);
    if (res != SUCCESS)
      return res;
  } else {
    res = parse_expr_or_call(buf, idx, ctx, &body_type);
    if (res != SUCCESS)
      return res;

    if (ctx->pass == 3)
      printf("POPS LF@%%retval\n");

    token_t end_token = tokenBufferGet(buf, *idx)->token_type;
    if (end_token == TOKEN_EOL)
      (*idx)++;
    else if (end_token != TOKEN_EOF)
      return SYN_ERR;
  }

  if (ctx->pass == 2) {
    res = check_function_body_end(ctx->curr_func, body_type, ret_all);
    if (res != SUCCESS)
      return res;

    ctx->curr_func->inferState = INFER_RESOLVED;

    if (ctx->curr_func->isMain && ctx->curr_func->returnType != DTYPE_UNIT)
      return TYPE_INC_SEM_ERR;

    ctx->curr_func->lastVarId = symTableGetNextVarId(ctx->st);

    symTablePopScope(ctx->st);
  }

  if (ctx->pass == 3) {
    if (ctx->curr_func->returnType == DTYPE_UNIT)
      printf("PUSHS nil@nil\n");
    else
      printf("PUSHS LF@%%retval\n");

    if (body_type == DTYPE_INT && ctx->curr_func->returnType == DTYPE_DOUBLE)
      generate_int2floats();

    generate_func_end();
    symTablePopScope(ctx->st);
  }

  return SUCCESS;
}

/**
 * @brief Verifies grammatical correctness of an if-statement
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param ret_out Output pointer indicating whether execution ended with return
 * @param data_type Output pointer for the resulting/parsed data type
 *
 * @return SUCCESS on success, or an error code
 */
int parse_if(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx, bool *ret_out,
             DType *data_type) {
  DType cond_type = DTYPE_UNKNOWN;
  DType then_type = DTYPE_UNIT, else_type = DTYPE_UNIT;
  bool ret_then = false, ret_else = false;
  int res;
  int if_id = 0;
  (*idx)++;

  SymTable *expr_st = NULL;
  if (ctx->pass == 2 || ctx->pass == 3)
    expr_st = ctx->st;

  // Check for condition in 2nd pass
  res = parse_expression(buf, idx, expr_st, &cond_type, ctx->pass == 3);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 2) {
    if (cond_type != DTYPE_BOOLEAN)
      return TYPE_INC_SEM_ERR;
  }

  if (ctx->pass == 3)
    if_id = generate_if_start();

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_THEN)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx, &then_type, &ret_then);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 3)
    generate_if_else(if_id);

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_ELSE)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx, &else_type, &ret_else);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 3)
    generate_if_end(if_id);

  if (ret_out != NULL)
    *ret_out = ret_then && ret_else;

  if (data_type != NULL) {
    if (((ret_then || then_type == DTYPE_UNIT) &&
         (ret_else || else_type == DTYPE_UNIT)) ||
        (ctx->pass == 2 && ctx->curr_func->hasExplicitReturn &&
         ctx->curr_func->returnType == DTYPE_UNIT))
      *data_type = DTYPE_UNIT;
    else
      *data_type = DTYPE_UNKNOWN;
  }
  return SUCCESS;
}

/**
 * @brief Verifies grammatical correctness of a while-loop
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
 */
int parse_while(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  DType cond_type = DTYPE_UNKNOWN;
  int res;
  int while_id = 0;
  (*idx)++;

  SymTable *expr_st = NULL;
  if (ctx->pass == 2 || ctx->pass == 3)
    expr_st = ctx->st;

  if (ctx->pass == 3)
    while_id = generate_while_start();

  // Check for condition in 2nd pass
  res = parse_expression(buf, idx, expr_st, &cond_type, ctx->pass == 3);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 2) {
    if (cond_type != DTYPE_BOOLEAN)
      return TYPE_INC_SEM_ERR;
  }

  if (ctx->pass == 3)
    generate_while_cond_end(while_id);

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_DO)
    return SYN_ERR;

  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
    return SYN_ERR;

  (*idx)++;

  res = parse_block(buf, idx, ctx, NULL, NULL);
  if (res != SUCCESS)
    return res;

  if (ctx->pass == 3)
    generate_while_end(while_id);

  return SUCCESS;
}

/**
 * @brief Verifies data type annotations
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param data_type Output pointer for the resulting/parsed data type
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
 */
int parse_type(const TokenBuffer *buf, size_t *idx, DType *data_type,
               ParserCtx *ctx) {
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
  } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID) {
    if (data_type != NULL)
      *data_type = DTYPE_UNKNOWN;

    (*idx)++;

    if (ctx->pass == 1) {
      record_sem_err(ctx, UNDEF_SEM_ERR);
      return SUCCESS;
    } else
      return UNDEF_SEM_ERR;
  }

  return SYN_ERR;
}

/**
 * @brief Verifies function parameter definitions
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param param_names Output array of parameter names
 * @param param_types Output array of parameter types
 * @param param_count Otuput pointer to the number of parsed parameters
 *
 * @return SUCCESS on success, or an error code
 */
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

    res = parse_type(buf, idx, &param_type, ctx);
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

/**
 * @brief Verifies block statements and literals
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param out_last_type Output pointer for the resulting type
 * @param ret_out Output pointer indicating whether execution ended with return
 *
 * @return SUCCESS on success, or an error code
 */
int parse_block(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                DType *out_last_type, bool *ret_out) {
  DType last_type = DTYPE_UNIT;
  bool block_ret = false;
  int res;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IND) {

    if (ctx->pass == 2 || ctx->pass == 3)
      symTablePushScope(ctx->st);

    (*idx)++;

    while (tokenBufferGet(buf, *idx)->token_type != TOKEN_DED) {
      if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL)
        (*idx)++;

      else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_VAR ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_VAL) {
        last_type = DTYPE_UNIT;
        block_ret = false;

        res = parse_variable(buf, idx, ctx);
        if (res != SUCCESS)
          return res;

        if (ctx->pass == 3)
          printf("MOVE LF@%%retval nil@nil\n");
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_IF) {
        bool ret_if = false;
        DType if_type = DTYPE_UNIT;

        res = parse_if(buf, idx, ctx, &ret_if, &if_type);
        if (res != SUCCESS)
          return res;

        if (ctx->pass == 3)
          printf("MOVE LF@%%retval nil@nil\n");

        last_type = if_type;
        block_ret = ret_if;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_WHILE) {
        last_type = DTYPE_UNIT;
        block_ret = false;

        res = parse_while(buf, idx, ctx);
        if (res != SUCCESS)
          return res;

        if (ctx->pass == 3)
          printf("MOVE LF@%%retval nil@nil\n");
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_RETURN) {
        last_type = DTYPE_UNIT;
        block_ret = true;

        res = parse_return(buf, idx, ctx);
        if (res != SUCCESS)
          return res;
      } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
                 peek_type(buf, idx) == TOKEN_ASSIGN) {
        char *name = tokenBufferGet(buf, *idx)->attribute;
        DType rhs_type = DTYPE_UNKNOWN;
        last_type = DTYPE_UNIT;
        block_ret = false;
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

        if (ctx->pass == 3) {
          Symbol *sym = symTableSearch(ctx->st, name);

          if (sym->type == DTYPE_DOUBLE && rhs_type == DTYPE_INT)
            generate_int2floats();

          printf("POPS LF@v$%zu\n", sym->varId);
          printf("MOVE LF@%%retval nil@nil\n");
        }

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        (*idx)++;
      } else {
        DType stmt_type = DTYPE_UNKNOWN;
        block_ret = false;
        res = parse_expr_or_call(buf, idx, ctx, &stmt_type);
        if (res != SUCCESS)
          return res;

        if (ctx->pass == 3)
          printf("POPS LF@%%retval\n");

        if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
          return SYN_ERR;

        last_type = stmt_type;
        (*idx)++;
      }
    }
    if (ctx->pass == 2 || ctx->pass == 3) {
      symTablePopScope(ctx->st);
    }
  } else
    return SYN_ERR;

  if (out_last_type != NULL)
    *out_last_type = last_type;

  if (ret_out != NULL)
    *ret_out = block_ret;

  (*idx)++;
  return SUCCESS;
}

/**
 * @brief Verifies return
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 *
 * @return SUCCESS on success, or an error code
 */
int parse_return(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx) {
  (*idx)++;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_EOL) {
    if (ctx->pass == 2) {
      int res = check_return_stmt(ctx->curr_func, false, DTYPE_UNIT);
      if (res != SUCCESS)
        return res;
    }

    if (ctx->pass == 3) {
      printf("PUSHS nil@nil\n");
      generate_func_end();
    }

    (*idx)++;
    return SUCCESS;
  } else {
    DType expr_type = DTYPE_UNKNOWN;
    int res = parse_expr_or_call(buf, idx, ctx, &expr_type);
    if (res != SUCCESS)
      return res;

    if (tokenBufferGet(buf, *idx)->token_type != TOKEN_EOL)
      return SYN_ERR;

    (*idx)++;

    if (ctx->pass == 2) {
      if (expr_type == DTYPE_BOOLEAN)
        return TYPE_INC_SEM_ERR;

      res = check_return_stmt(ctx->curr_func, true, expr_type);
      if (res != SUCCESS)
        return res;
    }

    if (ctx->pass == 3) {
      if (ctx->curr_func->returnType == DTYPE_UNIT) {
        printf("POPS GF@%%tmp1\n");
        printf("PUSHS nil@nil\n");
      } else if (expr_type == DTYPE_INT &&
                 ctx->curr_func->returnType == DTYPE_DOUBLE)
        generate_int2floats();

      generate_func_end();
    }

    return SUCCESS;
  }
}

/**
 * @brief Parses an expression or a function call
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param data_type Output pointer for the resulting expression type
 *
 * @return SUCCESS on success, or an error code
 */
int parse_expr_or_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                       DType *data_type) {
  SymTable *expr_st = NULL;
  if (ctx->pass == 2 || ctx->pass == 3)
    expr_st = ctx->st;

  if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID &&
      peek_type(buf, idx) == TOKEN_L_PARENTHESIS) {

    return parse_func_call(buf, idx, ctx, data_type);
  } else
    return parse_expression(buf, idx, expr_st, data_type, ctx->pass == 3);
}

/**
 * @brief Verifies function call
 *
 * @param buf Token buffer containing all input tokens
 * @param idx Pointer to the current index in the token buffer
 * @param ctx Parser context (symbol table, pass number, error state, current
 * function)
 * @param data_type Output pointer for the resulting expression type
 *
 * @return Parsed function call or expression TODO?
 */
int parse_func_call(const TokenBuffer *buf, size_t *idx, ParserCtx *ctx,
                    DType *data_type) {
  const char *func_name = tokenBufferGet(buf, *idx)->attribute;
  DType arg_types[TYPES];
  size_t arg_count = 0;
  Symbol *called_func = NULL;
  (*idx)++;
  (*idx)++;

  if (ctx->pass == 3)
    called_func = symTableSearch(ctx->st, func_name);

  while (tokenBufferGet(buf, *idx)->token_type != TOKEN_R_PARENTHESIS) {
    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_L_PARENTHESIS &&
        peek_type(buf, idx) == TOKEN_R_PARENTHESIS) {
      if (ctx->pass == 3)
        printf("PUSHS nil@nil\n");

      (*idx)++;
      (*idx)++;
      arg_types[arg_count] = DTYPE_UNIT;
    } else if (tokenBufferGet(buf, *idx)->token_type == TOKEN_ID ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_LIT_INT ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_LIT_DOUBLE ||
               tokenBufferGet(buf, *idx)->token_type == TOKEN_LIT_STRING) {
      if (ctx->pass == 2) {
        int err_code = SUCCESS;
        arg_types[arg_count] =
            get_term_type(tokenBufferGet(buf, *idx), ctx->st, &err_code);
        if (err_code != SUCCESS)
          return err_code;
      }

      if (ctx->pass == 3) {
        emit_push_term(tokenBufferGet(buf, *idx), ctx->st);
        int err_code = SUCCESS;
        DType arg_type =
            get_term_type(tokenBufferGet(buf, *idx), ctx->st, &err_code);

        if (arg_type == DTYPE_INT &&
            called_func->paramTypes[arg_count] == DTYPE_DOUBLE)
          generate_int2floats();
      }
      (*idx)++;
    } else
      return SYN_ERR;

    arg_count++;

    if (tokenBufferGet(buf, *idx)->token_type == TOKEN_COMMA) {
      if (peek_type(buf, idx) == TOKEN_R_PARENTHESIS)
        return SYN_ERR;

      (*idx)++;
    } else if (tokenBufferGet(buf, *idx)->token_type != TOKEN_R_PARENTHESIS)
      return SYN_ERR;
  }

  (*idx)++;

  if (ctx->pass == 2) {
    DType ret_type = DTYPE_UNKNOWN;
    int res = check_function_call(ctx->st, func_name, arg_count, arg_types,
                                  &ret_type);
    if (res != SUCCESS)
      return res;

    if (data_type != NULL)
      *data_type = ret_type;
  }

  if (ctx->pass == 3) {
    generate_call(func_name);

    if (data_type != NULL)
      *data_type = called_func->returnType;
  }

  return SUCCESS;
}

/** @brief Main entry point; initializes the token stream and begins
 *         recursive descent parsing
 */
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

  if (res != SUCCESS) {
    tokenBufferFree(buf);
    symTableFree(ctx.st);
    return res;
  }

  generate_program_start(ctx.main_name);
  ctx.pass = 3;
  idx = 0;
  res = parse_start(buf, &idx, &ctx);
  generate_program_end();

  tokenBufferFree(buf);
  symTableFree(ctx.st);

  return res;
}
