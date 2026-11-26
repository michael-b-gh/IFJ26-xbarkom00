/**
 * Project: IFJ26 Compiler
 *
 * @file expression.c
 * @brief Precedence syntax analysis for expressions
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#include "expression.h"
#include "codegen.h"
#include "error.h"
#include "semantic.h"
#include "token_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 9
#define LD_ARR 128
#define MULTI_QUOTE_LEN 3
#define SINGLE_QUOTE_LEN 1

prec_idx_t get_index(token_t token_type, stack_item_t *top);
int stack_push(stack_item_t **top, stack_item_type_t type, token_t tok_type,
               prec_idx_t idx, DType dtype);
void stack_pop(stack_item_t **top);
void stack_free_all(stack_item_t **top);
stack_item_t *find_top_terminal(stack_item_t *top);
int insert_marker(stack_item_t **top);
int reduce(stack_item_t **top, bool check_types, bool emit_code);
int parse_expression(const TokenBuffer *buf, size_t *idx, SymTable *symtable,
                     DType *data_type, bool emit_code);
void emit_push_term(const token *token, const SymTable *symtable);

prec_sign_t prec_table[TABLE_SIZE][TABLE_SIZE] = {
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_SH,
     PREC_RE},
    {PREC_SH, PREC_RE, PREC_RE, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_SH,
     PREC_RE},
    {PREC_SH, PREC_SH, PREC_ERR, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_SH,
     PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_ERR, PREC_SH, PREC_RE, PREC_SH, PREC_SH,
     PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_EQ, PREC_SH, PREC_SH,
     PREC_ERR},
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_ERR, PREC_RE, PREC_ERR, PREC_ERR,
     PREC_RE},
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_ERR, PREC_RE, PREC_ERR, PREC_ERR,
     PREC_RE},
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_SH,
     PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_ERR, PREC_SH, PREC_SH,
     PREC_ERR}};

/**
 * @brief Maps a lexer token to its corresponding precedence table index
 *
 * @param token_type Type of current token
 * @param top Pointer to the top of the stack
 *
 * @return Index for token in precedence table
 */
prec_idx_t get_index(token_t token_type, stack_item_t *top) {
  switch (token_type) {
  case TOKEN_MUL:
  case TOKEN_DIV:
    return IDX_MUL_DIV;

  case TOKEN_PLUS:
  case TOKEN_MINUS:
    if (top != NULL && (top->type == ITEM_NT_E || top->idx == IDX_OPD ||
                        top->idx == IDX_R_PAR))
      return IDX_ADD_SUB;
    else
      return IDX_UNARY;

  case TOKEN_LESS_THAN:
  case TOKEN_LESS_THAN_EQ:
  case TOKEN_MORE_THAN:
  case TOKEN_MORE_THAN_EQ:
    return IDX_REL;

  case TOKEN_EQ:
  case TOKEN_NOT_EQ:
    return IDX_EQ;

  case TOKEN_L_PARENTHESIS:
    return IDX_L_PAR;

  case TOKEN_R_PARENTHESIS:
    return IDX_R_PAR;

  case TOKEN_ID:
  case TOKEN_LIT_INT:
  case TOKEN_LIT_DOUBLE:
  case TOKEN_LIT_STRING:
    return IDX_OPD;

  default:
    return IDX_END;
  }
}

/**
 * @brief Pushes a new item onto the top of the stack
 *
 * @param top Pointer to pointer to the top of stack
 * @param type Kind of stack item (marker, terminal token, or non-terminal E)
 * @param tok_type Lexer token type (if item is a terminal)
 * @param idx Index of current token in precedence table
 * @param dtype Semantic data type of the item
 *
 * @return SUCCESS on success, or INT_ERR on malloc failure
 */
int stack_push(stack_item_t **top, stack_item_type_t type, token_t tok_type,
               prec_idx_t idx, DType dtype) {
  stack_item_t *new_node = malloc(sizeof(stack_item_t));

  if (new_node == NULL)
    return INT_ERR;

  new_node->type = type;
  new_node->token_type = tok_type;
  new_node->idx = idx;
  new_node->data_type = dtype;

  new_node->next = *top;
  *top = new_node;

  return SUCCESS;
}

/**
 * @brief Pops (removes) the top item in the stack
 *
 * @param top Pointer to pointer to the top of stack
 */
void stack_pop(stack_item_t **top) {
  if (*top == NULL)
    return;

  stack_item_t *tmp = *top;
  *top = tmp->next;

  free(tmp);
}

/**
 * @brief Frees all items remaining in the stack
 *
 * @param top Pointer to pointer to the top of the precedence stack
 */
void stack_free_all(stack_item_t **top) {
  while (*top != NULL)
    stack_pop(top);
}

/**
 * @brief Finds the topmost terminal on the stack (skips non-terminals)
 *
 * @param top Pointer to the top of the precedence stack
 *
 * @return Pointer to the topmmost terminal on the stack or NULL if none exists
 */
stack_item_t *find_top_terminal(stack_item_t *top) {
  while (top != NULL) {
    if (top->type == ITEM_TOKEN)
      return top;
    else
      top = top->next;
  }

  return NULL;
}

/**
 * @brief Inserts a handle marker (<) above the topmost terminal
 *
 * @param top Pointer to pointer to the top of the precedence stack
 *
 * @return SUCCESS on success, or an error code (INT_ERR, SYN_ERR)
 */
int insert_marker(stack_item_t **top) {
  if ((*top)->type == ITEM_TOKEN) {
    if (stack_push(top, ITEM_MARKER, 0, IDX_END, DTYPE_UNKNOWN) == SUCCESS)
      return SUCCESS;
    else
      return INT_ERR;
  } else if ((*top)->type == ITEM_NT_E) {
    stack_item_t *new_node = malloc(sizeof(stack_item_t));

    if (new_node == NULL)
      return INT_ERR;

    new_node->type = ITEM_MARKER;
    new_node->token_type = 0;
    new_node->idx = IDX_END;
    new_node->data_type = DTYPE_UNKNOWN;

    new_node->next = (*top)->next;
    (*top)->next = new_node;

    return SUCCESS;
  }

  return SYN_ERR;
}

/**
 * @brief Reduces the top handle (< ...) to non-terminal E, check types, and
 * optionally emits IFJcode26
 *
 * @param top Pointer to pointer to the top of the precedence stack
 * @param check_types If ture, performs semantic type checking during reduction
 * @param emit_code If true, emits IFJcode26 instructions to stdout
 *
 * @return SUCCESS on success, or an error code (SYN_ERR, TYPE_INC_SEM_ERR,
 * INT_ERR)
 */
int reduce(stack_item_t **top, bool check_types, bool emit_code) {
  stack_item_t *curr = *top;
  int cnt = 0;
  DType result_type = DTYPE_UNKNOWN;

  while (curr != NULL && curr->type != ITEM_MARKER) {
    cnt++;
    curr = curr->next;
  }

  if (curr == NULL)
    return SYN_ERR;

  if (cnt == 1) {
    if ((*top)->idx != IDX_OPD)
      return SYN_ERR;

    result_type = (*top)->data_type;
  } else if (cnt == 2) {
    stack_item_t *node_1st = *top;
    stack_item_t *node_2nd = node_1st->next;

    if (node_1st->idx == IDX_R_PAR && node_2nd->idx == IDX_L_PAR) {
      result_type = DTYPE_UNIT;

      if (emit_code)
        printf("PUSHS nil@nil\n");
    } else if (node_2nd->idx == IDX_UNARY && node_1st->type == ITEM_NT_E) {
      if (check_types)
        result_type = check_unary_op(node_1st->data_type, node_2nd->token_type);

      if (check_types) {
        if (result_type == DTYPE_UNKNOWN)
          return TYPE_INC_SEM_ERR;
      }

      if (emit_code && node_2nd->token_type == TOKEN_MINUS) {
        if (result_type == DTYPE_INT)
          generate_unary_minus_int();

        if (result_type == DTYPE_DOUBLE)
          generate_unary_minus_float();
      }
    } else
      return SYN_ERR;
  } else if (cnt == 3) {
    stack_item_t *node_1st = *top;
    stack_item_t *node_2nd = node_1st->next;
    stack_item_t *node_3rd = node_2nd->next;

    if (node_3rd->type == ITEM_NT_E && node_2nd->type == ITEM_TOKEN &&
        node_1st->type == ITEM_NT_E) {
      if (check_types)
        result_type = check_binary_op(node_3rd->data_type, node_1st->data_type,
                                      node_2nd->token_type);

      if (check_types) {
        if (result_type == DTYPE_UNKNOWN)
          return TYPE_INC_SEM_ERR;
      }

      if (emit_code) {
        DType left = node_3rd->data_type;
        DType right = node_1st->data_type;

        if (left == DTYPE_DOUBLE && right == DTYPE_INT)
          generate_int2floats();
        else if (left == DTYPE_INT && right == DTYPE_DOUBLE) {
          printf("POPS GF@%%tmp2\n");
          generate_int2floats();
          printf("PUSHS GF@%%tmp2\n");
        }

        switch (node_2nd->token_type) {
        case TOKEN_PLUS:
          if (result_type == DTYPE_STRING)
            generate_concats();
          else
            generate_adds();
          break;
        case TOKEN_MINUS:
          generate_subs();
          break;
        case TOKEN_MUL:
          generate_muls();
          break;
        case TOKEN_DIV:
          if (result_type == DTYPE_INT)
            generate_idivs();
          else
            generate_divs();
          break;
        case TOKEN_LESS_THAN:
          generate_lts();
          break;
        case TOKEN_MORE_THAN:
          generate_gts();
          break;
        case TOKEN_LESS_THAN_EQ:
          generate_le();
          break;
        case TOKEN_MORE_THAN_EQ:
          generate_ge();
          break;
        case TOKEN_EQ:
          generate_eqs();
          break;
        case TOKEN_NOT_EQ:
          generate_ne();
          break;

        default:
          break;
        }
      }
    } else if (node_3rd->idx == IDX_L_PAR && node_2nd->type == ITEM_NT_E &&
               node_1st->idx == IDX_R_PAR)
      result_type = node_2nd->data_type;
    else
      return SYN_ERR;
  } else
    return SYN_ERR;

  for (int i = 0; i != cnt + 1; i++)
    stack_pop(top);

  return stack_push(top, ITEM_NT_E, 0, IDX_END, result_type);
}

/**
 * @brief Emits a PUSHS instruction for a literal or variable token
 */
void emit_push_term(const token *tok, const SymTable *symtable) {
  if (tok->token_type == TOKEN_LIT_INT) {
    printf("PUSHS int@");

    for (int i = 0; tok->attribute[i] != '\0'; i++)
      if (tok->attribute[i] != '_')
        putchar(tok->attribute[i]);

    printf("\n");
  } else if (tok->token_type == TOKEN_LIT_DOUBLE) {
    char clean[LD_ARR];
    size_t ld_cntr = 0;

    for (int i = 0; tok->attribute[i] != '\0'; i++)
      if (tok->attribute[i] != '_' && tok->attribute[i] != 'd' &&
          tok->attribute[i] != 'D' && ld_cntr < 127)
        clean[ld_cntr++] = tok->attribute[i];

    clean[ld_cntr] = '\0';
    double val = strtod(clean, NULL);

    printf("PUSHS ");
    generate_float_literal(val);
  } else if (tok->token_type == TOKEN_LIT_STRING) {
    const char *str = tok->attribute;
    size_t len = strlen(str);
    size_t start = 0;

    if (len >= 2 * MULTI_QUOTE_LEN &&
        strncmp(str, "\"\"\"", MULTI_QUOTE_LEN) == 0)
      start = MULTI_QUOTE_LEN;
    else if (len >= 2 * SINGLE_QUOTE_LEN)
      start = SINGLE_QUOTE_LEN;

    printf("PUSHS string@");
    for (size_t i = start; i < len - start; i++) {
      int chtr = str[i];

      if (chtr <= 32 || chtr == '#' || chtr == '\\')
        printf("\\%03d", chtr);
      else
        putchar(chtr);
    }

    printf("\n");
  } else if (tok->token_type == TOKEN_ID) {
    Symbol *sym = symTableSearch(symtable, tok->attribute);

    if (sym != NULL)
      printf("PUSHS LF@v$%zu\n", sym->varId);
  }
}

/**
 * @brief Parses, type-checks, and optionally generates IFJcode26 for an
 * expression
 */
int parse_expression(const TokenBuffer *buf, size_t *idx, SymTable *symtable,
                     DType *data_type, bool emit_code) {
  stack_item_t *top = NULL;
  stack_push(&top, ITEM_TOKEN, 0, IDX_END, DTYPE_UNKNOWN);

  while (1) {
    token *current_token = tokenBufferGet(buf, *idx);
    stack_item_t *top_term = find_top_terminal(top);
    prec_idx_t input_idx = get_index(current_token->token_type, top);

    if (top_term->idx == IDX_END && input_idx == IDX_END) {
      if (top->type == ITEM_NT_E) {
        if (data_type != NULL)
          *data_type = top->data_type;

        stack_free_all(&top);
        return SUCCESS;
      } else {
        stack_free_all(&top);
        return SYN_ERR;
      }
    }

    if (prec_table[top_term->idx][input_idx] == PREC_EQ) {
      stack_push(&top, ITEM_TOKEN, current_token->token_type, input_idx,
                 DTYPE_UNKNOWN);
      (*idx)++;
    } else if (prec_table[top_term->idx][input_idx] == PREC_SH) {
      DType result_type = DTYPE_UNKNOWN;

      if (input_idx == IDX_OPD) {
        int err_code = SUCCESS;
        if (symtable != NULL)
          result_type = get_term_type(current_token, symtable, &err_code);

        if (err_code != SUCCESS) {
          stack_free_all(&top);
          return err_code;
        }

        if (emit_code)
          emit_push_term(current_token, symtable);
      }

      insert_marker(&top);
      stack_push(&top, ITEM_TOKEN, current_token->token_type, input_idx,
                 result_type);
      (*idx)++;
    } else if (prec_table[top_term->idx][input_idx] == PREC_RE) {
      int err = reduce(&top, symtable != NULL, emit_code);

      if (err != SUCCESS) {
        stack_free_all(&top);
        return err;
      }
    } else {
      stack_free_all(&top);
      return SYN_ERR;
    }
  }
}
