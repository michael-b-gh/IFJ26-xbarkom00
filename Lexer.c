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
#include <stdlib.h>

#include "error.h"
#include "scanner.h"

typedef enum {
  STATE_START,
  STATE_STRING,
  STATE_INT,
  STATE_COMMENT,
  STATE_ID,
  STATE_DOUBLE,
  STATE_MUL_STR,
  STATE_STRING_DONE,
  STATE_INT_DONE,
  STATE_COMMENT_DONE,
  STATE_ID_DONE,
  STATE_DOUBLE_DONE,
  STATE_MUL_STR_DONE,
  STATE_DO,
  STATE_DEF,
  STATE_ELSE,
  STATE_IF,
  STATE_IMPORT,
  STATE_RETURN,
  STATE_THEN,
  STATE_UNIT,
  STATE_VAL,
  STATE_VAR,
  STATE_WHILE,
  STATE_PLUS,
  STATE_MINUS,
  STATE_MUL,
  STATE_DIV,
  STATE_LESS_THAN,
  STATE_LESS_THAN_EQ,
  STATE_MORE_THAN,
  STATE_MORE_THAN_EQ,
  STATE_EQ,
  STATE_NOT_EQ,
  STATE_NOT,
  STATE_ASSIGN,
  STATE_IND,
  STATE_DED,
  STATE_EOL,
  STATE_EOF,
  STATE_L_PARENTHESIS,
  STATE_R_PARENTHESIS,
  STATE_COMMA,
  STATE_COLON,
  STATE_PROLOG
} states;

/**
 * @brief Takes symbol from stdin and gives it to the lexer
 *
 * @return Returns symbol from stdin to lexer
 */
int NextSymbol() {
  int symbol;
  symbol = getchar();
  return symbol;
}

int get_next_token(token* current_token) {
  char* word = malloc(10 * sizeof(char));
  int lenght = 0;
  int lenght_array = 10;
  int symbol = NextSymbol();
  states state = STATE_START;
  token Token;

  while (1) {
    if (lenght >= lenght_array) {
      char* check = realloc(word, (lenght_array * 2) * sizeof(char));
      if (check == NULL) {
        printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
        free(word);
        return LEX_ERR;
      }
      word = check;
      lenght_array = lenght_array * 2;
    }
    switch (state) {
      case STATE_START:
        if (isalpha(symbol) || symbol == '_') {
          state = STATE_ID;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (isdigit(symbol)) {
          state = STATE_INT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '\"') {
          state = STATE_STRING;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '+') {  // leaving more states so start state dont
                                     // return values imidiatelly
          state = STATE_PLUS;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '-') {
          state = STATE_MINUS;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '*') {
          state = STATE_MUL;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '/') {
          state = STATE_DIV;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '<') {
          state = STATE_LESS_THAN;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '>') {
          state = STATE_MORE_THAN;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '!') {
          state = STATE_NOT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '=') {
          state = STATE_ASSIGN;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '(') {
          state = STATE_L_PARENTHESIS;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == ')') {
          state = STATE_R_PARENTHESIS;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == ',') {
          state = STATE_COMMA;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == ':') {
          state = STATE_COLON;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        }  // TODO:add more state transitions
           // TODO:make transition for STATE_DOUBLE if there is .
        break;

      case STATE_PLUS:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_PLUS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_MINUS:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_MINUS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_MUL:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_MUL;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_DIV:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_DIV;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_LESS_THAN:
        if (symbol == '=') {
          state = STATE_LESS_THAN_EQ;
          word[lenght] = symbol;
          lenght++;
          break;
        } else {
          word[lenght] = '\0';
          current_token->token_type = TOKEN_LESS_THAN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
      case STATE_LESS_THAN_EQ:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_LESS_THAN_EQ;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_MORE_THAN:
        if (symbol == '=') {
          state = STATE_MORE_THAN_EQ;
          word[lenght] = symbol;
          lenght++;
          break;
        } else {
          word[lenght] = '\0';
          current_token->token_type = TOKEN_MORE_THAN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
      case STATE_MORE_THAN_EQ:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_MORE_THAN_EQ;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_NOT:
        if (symbol == '=') {
          state = STATE_NOT_EQ;
          word[lenght] = symbol;
          lenght++;
          break;
        } else if (symbol != ' ') {
          free(word);
          return LEX_ERR;
        }
      case STATE_NOT_EQ:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_NOT_EQ;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_ASSIGN:
        if (symbol == '=') {
          state = STATE_EQ;
          word[lenght] = symbol;
          lenght++;
          break;
        } else if (symbol = ' ') {
          word[lenght] = '\0';
          current_token->token_type = TOKEN_ASSIGN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else {
          return LEX_ERR;
        }
      case STATE_EQ:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_EQ;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_L_PARENTHESIS:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_L_PARENTHESIS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_R_PARENTHESIS:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_R_PARENTHESIS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_COMMA:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_COMMA;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_COLON:
        word[lenght] = '\0';
        current_token->token_type = TOKEN_COLON;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_ID:
        while (isalpha(symbol) || isdigit(symbol) || symbol == '_') {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return LEX_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          word[lenght] = symbol;
          symbol = NextSymbol();
          lenght++;
        }
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_ID;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_INT:
        while (isdigit(symbol) || symbol == '_') {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return LEX_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (symbol == 'e' || symbol == '.') {
            state = STATE_DOUBLE;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol = ' ') {
            word[lenght] = '\0';
            current_token->token_type = TOKEN_DOUBLE;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          } else {
            return LEX_ERR;
          }
        }
    }
  }
  return SUCCESS;
}