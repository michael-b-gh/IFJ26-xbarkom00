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
#include <string.h>

#include "error.h"
#include "scanner.h"

// TODO: implement DOUBLE from INT and STRING

typedef enum {
  STATE_START,
  STATE_STRING,
  STATE_INT,
  STATE_COMMENT,
  STATE_ID,
  STATE_DOUBLE,
  STATE_MUL_STR,
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
  STATE_PROLOG,
  STATE_EXP,
  STATE_EXP_SIGN,
  STATE_EXP_NUM,
  STATE_EXP_NUM_NO_E,
  STATE_INT_SIGN,
  STATE_ZERO,
  STATE_TWO_QUOTE,
  STATE_STRING_START,
  STATE_STR_CONTROL
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
  char last_symbol_word = ' ';
  int lenght = 0;
  int lenght_array = 10;
  int symbol = NextSymbol();
  states state = STATE_START;

  while (1) {
    if (lenght >= lenght_array) {
      char* check = realloc(word, (lenght_array * 2) * sizeof(char));
      if (check == NULL) {
        printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
        free(word);
        return INT_ERR;
      }
      word = check;
      lenght_array = lenght_array * 2;
    }
    switch (state) {
      case STATE_START:
        if (symbol == ' ' || symbol == '\n') {  // vygenerovane AI promptom:
                                                // Zasekava sa mi STATE_COMMENT
          symbol = NextSymbol();  // vygenerovane AI promptom: Zasekava sa mi
                                  // STATE_COMMENT
          break;  // vygenerovane AI promptom: Zasekava sa mi STATE_COMMENT
        } else if (isalpha(symbol) || symbol == '_') {
          state = STATE_ID;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '0') {
          state = STATE_ZERO;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (isdigit(symbol)) {
          state = STATE_INT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else if (symbol == '\"') {
          state = STATE_STRING_START;
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
        } else if (symbol == '.') {
          state = STATE_DOUBLE;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        }  // TODO:add more state transitions
        break;

      case STATE_PLUS:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_PLUS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_MINUS:
        if (isdigit(symbol)) {
          state = STATE_INT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          break;
        } else {
          ungetc(symbol, stdin);
          word[lenght] = '\0';
          current_token->token_type = TOKEN_MINUS;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
        break;
      case STATE_MUL:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_MUL;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_DIV:
        if (symbol == '*') {
          state = STATE_COMMENT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          break;
        }
        ungetc(symbol, stdin);
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
          ungetc(symbol, stdin);
          word[lenght] = '\0';
          current_token->token_type = TOKEN_LESS_THAN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
      case STATE_LESS_THAN_EQ:
        ungetc(symbol, stdin);
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
          ungetc(symbol, stdin);
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
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_EQ;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_L_PARENTHESIS:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_L_PARENTHESIS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_R_PARENTHESIS:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_R_PARENTHESIS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_COMMA:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_COMMA;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_COLON:
        ungetc(symbol, stdin);
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
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          word[lenght] = symbol;
          symbol = NextSymbol();
          lenght++;
        }
        word[lenght] = '\0';
        if (lenght == 1 && word[0] == '_') {
          free(word);
          lenght = 0;
          return LEX_ERR;
        } else if (strcmp(word, "def") == 0) {
          current_token->token_type = TOKEN_DEF;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "if") == 0) {
          current_token->token_type = TOKEN_IF;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "else") == 0) {
          current_token->token_type = TOKEN_ELSE;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "do") == 0) {
          current_token->token_type = TOKEN_DO;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "import") == 0) {
          current_token->token_type = TOKEN_IMPORT;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "return") == 0) {
          current_token->token_type = TOKEN_RETURN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "then") == 0) {
          current_token->token_type = TOKEN_THEN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "val") == 0) {
          current_token->token_type = TOKEN_VAL;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "var") == 0) {
          current_token->token_type = TOKEN_VAR;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "while") == 0) {
          current_token->token_type = TOKEN_WHILE;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else {
          current_token->token_type = TOKEN_ID;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
      case STATE_INT:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == '_') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (!isdigit(symbol)) {
              lenght = 0;
              free(word);
              return LEX_ERR;
            }
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == '.') {
            state = STATE_DOUBLE;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else if (symbol == 'e' || symbol == 'E') {
            state = STATE_EXP;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else if (symbol == 'l' || symbol == 'L') {
            free(word);
            lenght = 0;
            return LEX_ERR;
          } else {
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_INT;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          }
        }
        break;
      case STATE_ZERO:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (isdigit(symbol) || symbol == '_' || symbol == 'l' ||
              symbol == 'L') {
            free(word);
            lenght = 0;
            return LEX_ERR;
          } else if (symbol == 'e' || symbol == 'E') {
            state = STATE_EXP;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else if (symbol == '.') {
            state = STATE_DOUBLE;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else {
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_INT;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          }
        }
        break;
      case STATE_COMMENT:
        while (1) {
          if (last_symbol_word == '*' || symbol == '/') {
            state = STATE_START;
            lenght = 0;
            symbol = NextSymbol();
            break;
          } else {
            last_symbol_word = symbol;
            symbol = NextSymbol();
          }
        }
        continue;
      case STATE_DOUBLE:
        while (1) {
          if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            state = STATE_EXP_NUM_NO_E;
            break;
          } else if (symbol == 'e' || symbol == 'E') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            state = STATE_EXP;
            break;
          } else {
            ungetc(symbol, stdin);
            free(word);
            lenght = 0;
            return LEX_ERR;
          }
        }
        break;
      case STATE_EXP:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (symbol == 'e' || symbol == 'E') {
            lenght = 0;
            ungetc(symbol, stdin);
            free(word);
            return LEX_ERR;
          } else if (symbol == '+' || symbol == '-') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            state = STATE_EXP_SIGN;
            break;
          } else if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            state = STATE_EXP_NUM;
            break;
          } else {
            ungetc(symbol, stdin);
            lenght = 0;
            free(word);
            return LEX_ERR;
          }
        }
        break;
      case STATE_EXP_SIGN:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (symbol == '+' || symbol == '-') {
            lenght = 0;
            free(word);
            return LEX_ERR;
          } else if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            state = STATE_EXP_NUM;
            break;
          } else {
            ungetc(symbol, stdin);
            lenght = 0;
            free(word);
            return LEX_ERR;
          }
        }
        break;
      case STATE_EXP_NUM:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf(
                  "Pole nebolo mozne zvacsit, nie je mozne "
                  "pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == '_') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (!isdigit(symbol)) {
              lenght = 0;
              free(word);
              return LEX_ERR;
            }
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == 'L' || symbol == 'l') {
            free(word);
            lenght = 0;
            return LEX_ERR;
          } else if (symbol == 'd' || symbol == 'D') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (isdigit(symbol) || symbol == '.' || symbol == '_' ||
                symbol == 'e' || symbol == 'E' || isalpha(symbol)) {
              free(word);
              lenght = 0;
              return LEX_ERR;
            }
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_DOUBLE;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          } else if (symbol == 'f' || symbol == 'F') {
            ungetc(symbol, stdin);
            lenght = 0;
            free(word);
            return LEX_ERR;
          } else {
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_DOUBLE;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          }
        }
        break;
      case STATE_EXP_NUM_NO_E:
        while (1) {
          if (lenght >= lenght_array - 1) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf(
                  "Pole nebolo mozne zvacsit, nie je mozne "
                  "pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (isdigit(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == '_') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (!isdigit(symbol)) {
              lenght = 0;
              free(word);
              return LEX_ERR;
            }
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          } else if (symbol == 'e' || symbol == 'E') {
            state = STATE_EXP;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else if (symbol == 'd' || symbol == 'D') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (isdigit(symbol) || symbol == '.' || symbol == '_' ||
                symbol == 'e' || symbol == 'E' || isalpha(symbol)) {
              free(word);
              lenght = 0;
              return LEX_ERR;
            }
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_DOUBLE;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          } else if (symbol == 'f' || symbol == 'F') {
            ungetc(symbol, stdin);
            lenght = 0;
            free(word);
            return LEX_ERR;
          } else {
            ungetc(symbol, stdin);
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_DOUBLE;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          }
        }
        break;
      case STATE_STRING_START:
        if (symbol == '\"') {
          state = STATE_TWO_QUOTE;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        } else {
          state = STATE_STRING;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
        }
        break;
      case STATE_STRING:
        if (lenght >= lenght_array) {
          char* check = realloc(word, (lenght_array * 2) * sizeof(char));
          if (check == NULL) {
            printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
            free(word);
            return INT_ERR;
          }
          word = check;
          lenght_array = lenght_array * 2;
        }
        while (1) {
          if (symbol == '\"') {
            word[lenght] = symbol;
            lenght++;
            word[lenght] = '\0';
            current_token->token_type = TOKEN_LIT_STRING;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          } else if (symbol == '\n' || symbol == '\r') {
            free(word);
            lenght = 0;
            return LEX_ERR;
          } else if (symbol == '\\') {
            symbol = NextSymbol();
            if (symbol == 'n') {
              word[lenght] = '\n';
              lenght++;
              symbol = NextSymbol();
            } else if (symbol == 'r') {
              word[lenght] = '\r';
              lenght++;
              symbol = NextSymbol();
            } else if (symbol == 't') {
              word[lenght] = '\t';
              lenght++;
              symbol = NextSymbol();
            } else if (symbol == '\\') {
              word[lenght] = '\\';
              lenght++;
              symbol = NextSymbol();
            } else if (symbol == '\"') {
              word[lenght] = '\"';
              lenght++;
              symbol = NextSymbol();
            } else {
              free(word);
              lenght = 0;
              return LEX_ERR;
            }
          } else {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          }
        }
        break;
      case STATE_TWO_QUOTE:
        if (lenght >= lenght_array) {
          char* check = realloc(word, (lenght_array * 2) * sizeof(char));
          if (check == NULL) {
            printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
            free(word);
            return INT_ERR;
          }
          word = check;
          lenght_array = lenght_array * 2;
        }
        if (symbol == '\"') {
          state = STATE_MUL_STR;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          break;
        } else {
          word[lenght] = '\0';
          lenght = 0;
          current_token->token_type = TOKEN_LIT_STRING;
          current_token->attribute = word;
          return SUCCESS;
        }
      case STATE_MUL_STR:
        while (1) {
          if (lenght >= lenght_array) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (symbol == '\"') {
            state = STATE_STR_CONTROL;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          } else {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          }
        }
        break;
      case STATE_STR_CONTROL:
        while (1) {
          if (lenght >= lenght_array) {
            char* check = realloc(word, (lenght_array * 2) * sizeof(char));
            if (check == NULL) {
              printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
              free(word);
              return INT_ERR;
            }
            word = check;
            lenght_array = lenght_array * 2;
          }
          if (symbol == '\"') {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            if (symbol == '\"') {
              word[lenght] = symbol;
              lenght++;
              word[lenght] = '\0';
              lenght = 0;
              current_token->token_type = TOKEN_LIT_STRING;
              current_token->attribute = word;
              return SUCCESS;
            } else {
              state = STATE_MUL_STR;
              word[lenght] = symbol;
              lenght++;
              symbol = NextSymbol();
              break;
            }
          } else {
            state = STATE_MUL_STR;
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
            break;
          }
        }
        break;
    }
  }
  return SUCCESS;
}