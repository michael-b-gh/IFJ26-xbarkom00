/**
 * Project: IFJ26 Compiler
 *
 * @file expression.c
 * @brief Precedence syntax analysis for expressions
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#include "expression.h"
#include "error.h"
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 8

int consume_token(token *current_token);
prec_idx_t get_index(token_t token_type);
int stack_push(stack_item_t **top, stack_item_type_t type, token_t tok_type,
               prec_idx_t idx, DType dtype);
void stack_pop(stack_item_t **top);
void stack_free_all(stack_item_t **top);
stack_item_t *find_top_terminal(stack_item_t *top);
int insert_marker(stack_item_t **top);
int reduce(stack_item_t **top);
int parse_expression(token *current_token);

prec_sign_t prec_table[TABLE_SIZE][TABLE_SIZE] = {
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_RE},
    {PREC_SH, PREC_RE, PREC_RE, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_RE},
    {PREC_SH, PREC_SH, PREC_ERR, PREC_RE, PREC_SH, PREC_RE, PREC_SH, PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_ERR, PREC_SH, PREC_RE, PREC_SH, PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_EQ, PREC_SH, PREC_ERR},
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_ERR, PREC_RE, PREC_ERR, PREC_RE},
    {PREC_RE, PREC_RE, PREC_RE, PREC_RE, PREC_ERR, PREC_RE, PREC_ERR, PREC_RE},
    {PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_SH, PREC_ERR, PREC_SH, PREC_ERR}};

// Maps a lexer token to its corresponding precedence table index
prec_idx_t get_index(token_t token_type) {
  switch (token_type) {
  case TOKEN_MUL:
  case TOKEN_DIV:
    return IDX_MUL_DIV;

  case TOKEN_PLUS:
  case TOKEN_MINUS:
    return IDX_ADD_SUB;

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
  case TOKEN_UNIT:
    return IDX_OPD;

  default:
    return IDX_END;
  }
}

// Pushes a new item onto the top of the stack
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

// Pops (removes) the top item in the stack
void stack_pop(stack_item_t **top) {
  if (*top == NULL)
    return;

  stack_item_t *tmp = *top;
  *top = tmp->next;

  free(tmp);
}

// Frees all items remaining in the stack
void stack_free_all(stack_item_t **top) {
  while (*top != NULL)
    stack_pop(top);
}

// Finds the topmost terminal on the stack (skips non-terminals)
stack_item_t *find_top_terminal(stack_item_t *top) {
  while (top != NULL) {
    if (top->type == ITEM_TOKEN)
      return top;
    else
      top = top->next;
  }

  return NULL;
}

// Inserts a handle marker (<) above the topmost terminal
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

// Reduces the top handle (< ...) to a single non-terminal E
int reduce(stack_item_t **top) {
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
  } else if (cnt == 3) {
    stack_item_t *node_1st = *top;
    stack_item_t *node_2nd = node_1st->next;
    stack_item_t *node_3rd = node_2nd->next;

    if (node_3rd->type == ITEM_NT_E && node_2nd->type == ITEM_TOKEN &&
        node_1st->type == ITEM_NT_E) {
      result_type = check_binary_op(node_3rd->data_type, node_1st->data_type,
                                    node_2nd->token_type);

      if (result_type == DTYPE_UNKNOWN)
        return TYPE_INC_SEM_ERR;
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

// Parses and type-checks an expression using operator-precedence parsing
int parse_expression(token *current_token) {
  stack_item_t *top = NULL;
  stack_push(&top, ITEM_TOKEN, 0, IDX_END, DTYPE_UNKNOWN);

  while (1) {
    stack_item_t *top_term = find_top_terminal(top);
    prec_idx_t input_idx = get_index(current_token->token_type);

    if (top_term->idx == IDX_END && input_idx == IDX_END) {
      if (top->type == ITEM_NT_E) {
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
      consume_token(current_token);
    } else if (prec_table[top_term->idx][input_idx] == PREC_SH) {
      insert_marker(&top);
      stack_push(&top, ITEM_TOKEN, current_token->token_type, input_idx,
                 DTYPE_UNKNOWN);
      consume_token(current_token);
    } else if (prec_table[top_term->idx][input_idx] == PREC_RE) {
      int err = reduce(&top);

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
