/**
* Project: IFJ26 Compiler
 *
 * @file semantic.h
 * @brief Interface for type checking
 * @author Lukas Zelenak (xzelenl00)
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Checks the types of binary operation (a+b)
 *
 * @param leftType left operand type
 * @param rightType right operand type
 * @param op operator type of token from lexer
 *
 * @return Result type of the operation
 * DTYPE_UNKNOWN if the operation is not allowed for these types
 */
DType check_binary_op(DType leftType, DType rightType, token_t op);

/**
 * @brief Checks the types of unary operation (-a)
 *
 * @param operandType operand type
 * @param op operator type of token from lexer
 *
 * @return Result type of the operation
 * DTYPE_UNKNOWN if the operation is not allowed for this type
 */
DType check_unary_op(DType operandType, token_t op);

#endif // SEMANTIC_H
