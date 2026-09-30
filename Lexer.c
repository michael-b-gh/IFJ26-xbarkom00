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

int main() {
  char* word = malloc(10 * sizeof(char));
  int lenght = 0;
  int lenght_array = 10;
  int symbol = NextSymbol;
  states state = STATE_START;
  token Token;

  while (1) {
    if (lenght > lenght_array) {
      char* check = realloc(word, (lenght_array * 2) * sizeof(char));
      if (check == NULL) {
        printf("Pole nebolo mozne zvacsit, nie je mozne pokracovat.");
        free(word);
        return 1;
      }
      word = check;
      lenght_array = lenght_array * 2;
    }
  }
  case(state){
    if (isalpha(symbol)){
      state=STATE_ID;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(isdigit(symbol)){
      state=STATE_INT;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(symbol=='\"'){
      state=STATE_STRING;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(symbol=='+'){ //leaving more states so start state dont return values imidiatelly
      state=STATE_PLUS;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(symbol=='-'){
      state=STATE_MINUS;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(symbol=='*'){
      state=STATE_MUL;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
    }else if(symbol=='/'){
      state=STATE_DIV;
      word[lenght_array]=symbol;
      lenght++;
      symbol=NextSymbol;
  }//add more state transitions for <,>,and such
  
}
}