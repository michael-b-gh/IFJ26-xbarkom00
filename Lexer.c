/**
 * Project: IFJ26 Compiler
 *
 * @file Lexer.c
 * @brief Lexer(Lexical analyzer) takes text on input and translate it into
 * Tokens for parser
 * @author Juraj Pauček (xpaucej00)
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlin.h>
#include scanner.h

typedef enum {
  STATE_START,
  STATE_STRING,
  STATE_INT,
  STATE_COMMENT,
  STATE_ID,
  STATE_DOUBLE,
  STATE_MUL_STR,
  STATE_TOKEN_DONE
} states;

typedef struct {
} ret_symbol;

int NextSymbol() {
  int symbol;
  int row = 0;
  symbol = getchar();
  if (symbol == '\n') {
    row++;
  }
  return symbol;
  return row;
}

int main() {}