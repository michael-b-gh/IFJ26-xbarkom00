/**
 * Project: IFJ26 Compiler
 *
 * @file scanner.h
 * @brief Scanner represents lexems by uniform tokens
 * @author Mykhailo Barkovskyi (xbarkom00)
 * @author Juraj Pauček (xpaucej00)
 */

#ifndef _SCANNER_IFJ26_H
#define _SCANNER_IFJ26_H

typedef enum {
  TOKEN_DEF,
  TOKEN_DO,
  TOKEN_DOUBLE,
  TOKEN_ELSE,
  TOKEN_IF,
  TOKEN_IMPORT,
  TOKEN_INT,
  TOKEN_RETURN,
  TOKEN_STRING,
  TOKEN_THEN,
  TOKEN_UNIT,
  TOKEN_VAL,
  TOKEN_VAR,
  TOKEN_WHILE,
  TOKEN_ID,
  TOKEN_LIT_INT,
  TOKEN_LIT_DOUBLE,
  TOKEN_LIT_STRING,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_LESS_THAN,
  TOKEN_LESS_THAN_EQ,
  TOKEN_MORE_THAN,
  TOKEN_MORE_THAN_EQ,
  TOKEN_EQ,
  TOKEN_NOT_EQ,
  TOKEN_ASSIGN,
  TOKEN_IND,
  TOKEN_DED,
  TOKEN_EOL,
  TOKEN_EOF,
  TOKEN_L_PARENTHESIS,
  TOKEN_R_PARENTHESIS,
  TOKEN_COMMA,
  TOKEN_COLON,
  TOKEN_PROLOG,
  TOKEN_MAIN
} token_t;

typedef struct {
  token_t token_type;
  char* attribute;
} token;

int get_next_token(token* current_token);

#endif
