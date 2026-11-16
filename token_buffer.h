/**
* Project: IFJ26 Compiler
 *
 * @file token_buffer.h
 * @brief Dynamic array of all tokens that are loaded by lexer before parsing.
 * @author Lukas Zelenak (xzelenl00)
 */

#ifndef PROJEKT_TOKEN_BUFFER_H
#define PROJEKT_TOKEN_BUFFER_H

#include "scanner.h"

#include <stddef.h>

typedef struct TokenBuffer TokenBuffer;

/**
 * @brief Allocates empty buffer. If allocation fails program exits with
 * INT_ERR.
 *
 * @return New allocated buffer.
 */
TokenBuffer *tokenBufferCreate(void);

/**
 * @brief Loads all tokens from lexer until the EOF.
 *
 * @param tokenbuffer Buffer to load in.
 *
 * @return SUCCESS or the lexer error code LEX_ERR or INT_ERR.
 */
int tokenBufferLoad(TokenBuffer *tokenbuffer);

/**
 * @brief Returns pointer to the token at index idx. Call only after
 * tokenBufferLoad.
 *
 * @param tokenbuffer Buffer to read from.
 * @param idx Index of the token.
 *
 * @return Pointer to token or NULL if index is out of range.
 */
token *tokenBufferGet(const TokenBuffer *tokenbuffer, size_t idx);

/**
 * @brief Returns the number of tokens in the buffer.
 *
 * @param tokenbuffer Buffer to count tokens in.
 *
 * @return Number of loaded tokens
 */
size_t tokenBufferCount(const TokenBuffer *tokenbuffer);

/**
 * @brief Frees created buffer and all of token attributes. If NULL nothing will
 * be freed.
 *
 * @param tokenbuffer Buffer to be freed
 */
void tokenBufferFree(TokenBuffer *tokenbuffer);

#endif // PROJEKT_TOKEN_BUFFER_H
