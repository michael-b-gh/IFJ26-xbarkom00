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

/**
 * @brief Returns data type of operand in expression.
 *
 * @param tok operand token
 * @param symtable table to search the variable in
 * @param err_code SUCCESS on success or UNDEF_SEM_ERR if the variable
 * doesn't exist or it is a function
 *
 * @return DTYPE of operand or DTYPE_UNKNOWN if the variable doesn't exist or
 * token is not operand
 */
DType get_term_type(token *tok, SymTable *symtable, int *err_code);

#endif // SEMANTIC_H
