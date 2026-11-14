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
  STATE_EXP,
  STATE_EXP_SIGN,
  STATE_EXP_NUM,
  STATE_EXP_NUM_NO_E,
  STATE_INT_SIGN,
  STATE_ZERO,
  STATE_TWO_QUOTE,
  STATE_STRING_START,
  STATE_STR_CONTROL,
  STATE_MUL_COMMENT,
  STATE_DOT
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
  int comm_counter = 0;
  int comm_mul_line = 0;
  static int parent_count = 0;
  static int dent_arr[50] = {0};
  static int new_line = 1;
  static int dent_index = 0;
  static int last_before_parent = 0;
  int counter = 0;
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

    if (new_line == 1) {
      while (symbol == ' ') {
        counter++;
        symbol = NextSymbol();
      }

      if (parent_count >
          0) {  // vygenerovane AI promptom: Ako preskocim rovno na moj switch
        if (counter <= last_before_parent) {
          free(word);
          return LEX_ERR;
        }
        new_line = 0;
        goto skip_ind;
      }

      if (symbol == '\n' || symbol == EOF) {
        new_line = 1;
      } else {
        int last_ind;
        if (dent_index == 0) {
          last_ind = 0;
        } else {
          last_ind = dent_arr[dent_index - 1];
        }
        if (counter > last_ind) {
          dent_arr[dent_index] = counter;
          last_before_parent = dent_arr[dent_index] = counter;
          dent_index++;
          new_line = 0;
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_IND;
          current_token->attribute = NULL;
          return SUCCESS;
        } else if (counter < last_ind) {
          if (dent_index > 0) {
            dent_index--;
          }
          ungetc(symbol, stdin);
          new_line = 0;
          current_token->token_type = TOKEN_DED;
          current_token->attribute = NULL;
          return SUCCESS;
        } else {
          new_line = 0;
        }
      }
    }

  skip_ind:
    switch (state) {
      case STATE_START:
        if (symbol == ' ') {      // vygenerovane AI promptom:
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
        } else if (symbol == '+') {
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
          parent_count++;
          state = STATE_L_PARENTHESIS;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          break;
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
          state = STATE_DOT;
          word[lenght] = symbol;
          lenght++;
        } else if (symbol == '@') {
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          while (isalpha(symbol)) {
            word[lenght] = symbol;
            lenght++;
            symbol = NextSymbol();
          }
          if (strcmp(word, "@main") == 0) {
            word[lenght] = '\0';
            ungetc(symbol, stdin);
            current_token->token_type = TOKEN_MAIN;
            current_token->attribute = word;
            return SUCCESS;
          } else {
            ungetc(symbol, stdin);
            lenght = 0;
            free(word);
            return LEX_ERR;
          }
        } else if (symbol == '\n') {
          if (parent_count > 0) {
            new_line = 1;
            symbol = NextSymbol();
            continue;
          } else {
            new_line = 1;
            current_token->token_type = TOKEN_EOL;
            current_token->attribute = NULL;
            return SUCCESS;
          }
        } else if (symbol == EOF) {
          current_token->token_type = TOKEN_EOF;
          current_token->attribute = NULL;
          return SUCCESS;
        } else {
          free(word);
          lenght = 0;
          return LEX_ERR;
        }
        break;
      case STATE_PLUS:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_PLUS;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_MINUS:
          ungetc(symbol, stdin);
          word[lenght] = '\0';
          current_token->token_type = TOKEN_MINUS;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        break;
      case STATE_MUL:
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_MUL;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_DOT:
        symbol = NextSymbol();
        if (isdigit(symbol)) {
          state = STATE_DOUBLE;
          word[lenght] = symbol;
          break;
        }
        ungetc(symbol, stdin);
        word[lenght] = '\0';
        current_token->token_type = TOKEN_DOT;
        current_token->attribute = word;
        lenght = 0;
        return SUCCESS;
      case STATE_DIV:
        if (symbol == '*') {
          state = STATE_MUL_COMMENT;
          word[lenght] = symbol;
          lenght++;
          symbol = NextSymbol();
          break;
        } else if (symbol == '/') {
          state = STATE_COMMENT;
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
        } else {
          ungetc(symbol, stdin);
          word[lenght] = '\0';
          current_token->token_type = TOKEN_ASSIGN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        }
      case STATE_EQ:
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
        break;
      case STATE_R_PARENTHESIS:
        parent_count--;
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
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_DEF;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "if") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_IF;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "else") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_ELSE;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "do") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_DO;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "import") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_IMPORT;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "return") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_RETURN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "then") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_THEN;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "val") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_VAL;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "var") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_VAR;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "while") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_WHILE;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "Double") == 0) {
          current_token->token_type = TOKEN_DOUBLE;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "String") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_STRING;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "Int") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_INT;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else if (strcmp(word, "Unit") == 0) {
          ungetc(symbol, stdin);
          current_token->token_type = TOKEN_UNIT;
          current_token->attribute = word;
          lenght = 0;
          return SUCCESS;
        } else {
          ungetc(symbol, stdin);
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
            current_token->token_type = TOKEN_LIT_INT;
            current_token->attribute = word;
            lenght = 0;
            return SUCCESS;
          }
        }
        break;
      case STATE_COMMENT:
        while (symbol != '\n' || symbol == EOF) {
          symbol = NextSymbol();
        }
        state = STATE_START;
        lenght = 0;
        new_line = 1;
        current_token->token_type = TOKEN_EOL;
        current_token->attribute = NULL;
        return SUCCESS;
      case STATE_MUL_COMMENT:
        comm_counter++;
        while (comm_counter > 0) {
          if (symbol == EOF) {
            free(word);
            lenght = 0;
            return LEX_ERR;
          } else if (symbol == '\n') {
            comm_mul_line = 1;
          }
          if (last_symbol_word == '*' && symbol == '/') {
            comm_counter--;
            symbol = NextSymbol();
            continue;
          } else if (last_symbol_word == '/' && symbol == '*') {
            comm_counter++;
            symbol = NextSymbol();
            continue;
          } else {
            last_symbol_word = symbol;
            symbol = NextSymbol();
          }
        }
        state = STATE_START;
        if (comm_mul_line == 0) {
          lenght = 0;
          continue;
        } else {
          while (symbol != EOF &&
                 symbol != '\n') {  // vygenerovane AI promptom: Zasekava sa mi
                                    // blokovi komentar
            if (symbol != ' ' && symbol != '\t' &&
                symbol != '\r') {  // vygenerovane AI promptom: Zasekava sa mi
                                   // blokovi komentar
              free(word);
              return LEX_ERR;
            }
            symbol = NextSymbol();
          }
          new_line = 1;
          current_token->token_type = TOKEN_EOL;
          current_token->attribute = NULL;
          return SUCCESS;
        }
        break;
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
          ungetc(symbol, stdin);
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
