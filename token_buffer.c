/**
* Project: IFJ26 Compiler
 *
 * @file token_buffer.c
 * @brief Implementation of token buffer.
 * @author Lukas Zelenak (xzelenl00)
 */

#include "token_buffer.h"
#include "error.h"

#include <stdlib.h>

#define TOKEN_BUFFER_INITIAL_SIZE 100

struct TokenBuffer {
  token *tokens;
  size_t count;
  size_t capacity;
};

static void tokenBufferPush(TokenBuffer *buffer, token t) {
  if (buffer->count == buffer->capacity) {
    size_t newCapacity = buffer->capacity * 2;
    token *newTokens = realloc(buffer->tokens, newCapacity * sizeof(token));
    if (newTokens == NULL)
      exit(INT_ERR);

    buffer->tokens = newTokens;
    buffer->capacity = newCapacity;
  }
  buffer->tokens[buffer->count] = t;
  buffer->count++;
}

TokenBuffer *tokenBufferCreate(void) {
  TokenBuffer *buffer = malloc(sizeof(TokenBuffer));
  if (buffer == NULL)
    exit(INT_ERR);
  buffer->tokens = calloc(TOKEN_BUFFER_INITIAL_SIZE, sizeof(token));
  if (buffer->tokens == NULL) {
    free(buffer);
    exit(INT_ERR);
  }
  buffer->count = 0;
  buffer->capacity = TOKEN_BUFFER_INITIAL_SIZE;
  return buffer;
}

int tokenBufferLoad(TokenBuffer *tokenbuffer) {
  while (1) {
    token t;
    int result = get_next_token(&t);
    if (result != SUCCESS)
      return result;
    tokenBufferPush(tokenbuffer, t);
    if (t.token_type == TOKEN_EOF)
      return SUCCESS;
  }
}

token *tokenBufferGet(const TokenBuffer *tokenbuffer, size_t idx) {
  if (idx >= tokenbuffer->count) {
    return NULL;
  } else {
    return &tokenbuffer->tokens[idx];
  }
}

size_t tokenBufferCount(const TokenBuffer *tokenbuffer) {
  return tokenbuffer->count;
}

void tokenBufferFree(TokenBuffer *tokenbuffer) {
  if (tokenbuffer != NULL) {
    for (size_t i = 0; i < tokenbuffer->count; i++) {
      free(tokenbuffer->tokens[i].attribute);
    }
    free(tokenbuffer->tokens);
    free(tokenbuffer);
  }
}
