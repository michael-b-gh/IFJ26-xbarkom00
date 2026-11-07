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

/**
 * @brief Verifies if variable exists and is mutable. (x=..)
 *
 * @param symtable table to search the variable in.
 * @param name identifier for variable on the left side.
 *
 * @return UNDEF_SEM_ERR if variable doesn't exist or it is a function.
 *  VAR_SEM_ERR if variable is not mutable.
 *  SUCCESS if variable is mutable.
 */
int check_assignment(SymTable *symtable, const char *name);

/**
 * @brief Verifies that \@main has no parameters and returns DTYPE_UNIT.
 *
 * @param paramCount number of parameters of the function.
 * @param returnType return type of the function.
 *
 * @return  PARAM_SEM_ERR if number of parameters is not 0.
 * TYPE_INC_SEM_ERR if return type is not DTYPE_UNIT.
 * SUCCESS if function has number of parameters 0 and return type is not
 * DTYPE_UNIT.
 */
int check_main_function(size_t paramCount, DType returnType);

#endif // SEMANTIC_H
