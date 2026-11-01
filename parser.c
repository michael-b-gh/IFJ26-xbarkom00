/**
 * Project: IFJ26 Compiler
 *
 * @file parser.c
 * @brief Parser verifies that the string of tokens represents a syntactically
 * well-formed program
 * @author Mykhailo Barkovskyi (xbarkom00)
 */

#include <stdio.h>

// Stores the current token to allow multiple checks without advancing the tape
// head
#include "error.h"
#include "scanner.h"

int consume_token(token *current_token);
int parse_start(token *current_token);
int parse_prolog(token *current_token);
int parse_variable(token *current_token);
int parse_function(token *current_token);
int parse_if(token *current_token);
int parse_while(token *current_token);
int parse_type(token *current_token);
int parse_param(token *current_token);
int parse_block(token *current_token);
// TODO: int parse_return(token *current_token);

int consume_token(token *current_token) {
  if (current_token->attribute != NULL) {
    free(current_token->attribute);
    current_token->attribute = NULL;
  }

  return get_next_token(current_token);
}

// Top-level driver for recursive descent parsing
int parse_start(token *current_token) {
  if (parse_prolog(current_token) == SYN_ERR)
    return SYN_ERR;

  while (current_token->token_type != TOKEN_EOF) {
    if (current_token->token_type == TOKEN_VAR ||
        current_token->token_type == TOKEN_VAL) {
      if (parse_variable(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_DEF) {
      if (parse_function(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_IF) {
      if (parse_if(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_WHILE) {
      if (parse_while(current_token) == SYN_ERR)
        return SYN_ERR;
    } else
      return SYN_ERR;
  }

  return SUCCESS;
}

// Verifies grammatical correctness of the prolog
int parse_prolog(token *current_token) {
  if (current_token->token_type == TOKEN_PROLOG) {
    consume_token(current_token);

    if (current_token->token_type == TOKEN_EOL) {
      consume_token(current_token);
      return SUCCESS;
    }
  }

  return SYN_ERR;
}

// Verifies grammatical correctness of the variable definition
int parse_variable(token *current_token) {
  consume_token(current_token);

  if (current_token->token_type == TOKEN_ID) {
    consume_token(current_token);

    if (current_token->token_type == TOKEN_COLON) {
      consume_token(current_token);
      if (parse_type(current_token) == SYN_ERR)
        return SYN_ERR;
    }

    if (current_token->token_type == TOKEN_ASSIGN) {
      // TODO: Check for expression or function call
      consume_token(current_token);

      if (current_token->token_type == TOKEN_EOL) {
        consume_token(current_token);
        return SUCCESS;
      }
    } else
      return SYN_ERR;
  }

  return SYN_ERR;
}

// Verifies grammatical correctness of the function definition
int parse_function(token *current_token) {
  consume_token(current_token);

  if (current_token->token_type == TOKEN_ID) {
    consume_token(current_token);

    if (current_token->token_type == TOKEN_L_PARENTHESIS) {
      consume_token(current_token);
      if (parse_param(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_COLON) {
      if (parse_type(current_token) == SYN_ERR)
        return SYN_ERR;

      consume_token(current_token);

    } else if (current_token->token_type == TOKEN_ASSIGN) {
      consume_token(current_token);

      if (current_token->token_type == TOKEN_EOL) {
        consume_token(current_token);

        if (current_token->token_type == TOKEN_IND) {
          if (parse_block(current_token) == SYN_ERR)
            return SYN_ERR;
          consume_token(current_token);

          if (current_token->token_type == TOKEN_DED) {
            consume_token(current_token);
            return SUCCESS;
          }
        }
      }
    } else
      return SYN_ERR;
  }

  return SYN_ERR;
}

// Verifies grammatical correctness of an if-statement
int parse_if(token *current_token) {
  consume_token(current_token);
  // TODO: Check for condition

  if (current_token->token_type == TOKEN_THEN) {
    consume_token(current_token);

    if (current_token->token_type == TOKEN_EOL) {
      consume_token(current_token);

      if (current_token->token_type == TOKEN_IND) {
        consume_token(current_token);
        if (parse_block(current_token) == SYN_ERR)
          return SYN_ERR;
        if (current_token->token_type == TOKEN_DED) {
          consume_token(current_token);

          if (current_token->token_type == TOKEN_ELSE) {
            consume_token(current_token);
            if (current_token->token_type == TOKEN_EOL) {
              consume_token(current_token);

              if (current_token->token_type == TOKEN_IND) {
                consume_token(current_token);
                if (parse_block(current_token) == SYN_ERR)
                  return SYN_ERR;

                if (current_token->token_type == TOKEN_DED) {
                  consume_token(current_token);
                  return SUCCESS;
                }
              }
            }
          }
        }
      }

    } else
      return SYN_ERR;
  }

  return SYN_ERR;
}

// Verifies grammatical correctness of a while-loop
int parse_while(token *current_token) {
  consume_token(current_token);

  if (current_token->token_type == TOKEN_DO) {
    // TODO: Check for condition
    consume_token(current_token);

    if (current_token->token_type == TOKEN_EOL) {
      consume_token(current_token);

      if (current_token->token_type == TOKEN_IND) {
        if (parse_block(current_token) == SYN_ERR)
          return SYN_ERR;
        consume_token(current_token);

        if (current_token->token_type == TOKEN_DED) {
          consume_token(current_token);
          return SUCCESS;
        } else
          return SYN_ERR;
      } else
        return SYN_ERR;
    }
  }

  return SYN_ERR;
}

// Verifies data type annotations
int parse_type(token *current_token) {
  if (current_token->token_type == TOKEN_INT ||
      current_token->token_type == TOKEN_DOUBLE ||
      current_token->token_type == TOKEN_STRING ||
      current_token->token_type == TOKEN_UNIT) {

    consume_token(current_token);

    return SUCCESS;
  }

  return SYN_ERR;
}

// Verifies function parameter definitions
int parse_param(token *current_token) {
  if (current_token->token_type == TOKEN_R_PARENTHESIS) {
    consume_token(current_token);
    return SUCCESS;
  }

  while (current_token->token_type != TOKEN_R_PARENTHESIS) {
    if (current_token->token_type == TOKEN_ID)
      consume_token(current_token);
    else
      return SYN_ERR;

    if (current_token->token_type == TOKEN_COLON)
      consume_token(current_token);
    else
      return SYN_ERR;

    if (parse_type(current_token) == SYN_ERR)
      return SYN_ERR;

    if (current_token->token_type == TOKEN_COMMA)
      consume_token(current_token);
    else if (current_token->token_type == TOKEN_R_PARENTHESIS) {
      // Parser hit ")": termination of the loop
    } else
      return SYN_ERR;
  }
  consume_token(current_token);
  return SUCCESS;
}

// Verifies block statements
int parse_block(token *current_token) {
  consume_token(current_token);

  while (current_token->token_type != TOKEN_DED) {
    if (current_token->token_type == TOKEN_VAR ||
        current_token->token_type == TOKEN_VAL) {
      if (parse_variable(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_DEF) {
      if (parse_function(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_IF) {
      if (parse_if(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_WHILE) {
      if (parse_while(current_token) == SYN_ERR)
        return SYN_ERR;

    } else if (current_token->token_type == TOKEN_RETURN) {
      // TODO: implement parse_return();
      // if (parse_return(current_token) == SYN_ERR)
      //    return SYN_ERR;

    } else if (current_token->token_type == TOKEN_ID) {
      // TODO: differentiate between variable reassignment and function call
    } else
      return SYN_ERR;
  }

  consume_token(current_token);
  return SUCCESS;
}

// @brief Main entry point; initializes the token stream and begins recursive
// descent parsing
int main() {
  token current_token;
  current_token.attribute = NULL;
  consume_token(&current_token);

  if (parse_start(&current_token) == SUCCESS)
    return SUCCESS;
  else
    return SYN_ERR;
}
