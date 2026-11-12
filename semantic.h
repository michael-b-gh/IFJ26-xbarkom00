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
 * @brief Checks the types of binary operation (a+b).
 *
 * @param leftType left operand type.
 * @param rightType right operand type.
 * @param op operator type of token from lexer.
 *
 * @return Result type of the operation
 * DTYPE_UNKNOWN if the operation is not allowed for these types.
 */
DType check_binary_op(DType leftType, DType rightType, token_t op);

/**
 * @brief Checks the types of unary operation (-a).
 *
 * @param operandType operand type.
 * @param op operator type of token from lexer.
 *
 * @return Result type of the operation.
 * DTYPE_UNKNOWN if the operation is not allowed for this type.
 */
DType check_unary_op(DType operandType, token_t op);

/**
 * @brief Returns data type of operand in expression.
 *
 * @param tok operand token.
 * @param symtable table to search the variable in.
 * @param err_code SUCCESS on success or UNDEF_SEM_ERR if the variable
 * doesn't exist or it is a function.
 *
 * @return DTYPE of operand or DTYPE_UNKNOWN if the variable doesn't exist or
 * token is not operand.
 */
DType get_term_type(token *tok, SymTable *symtable, int *err_code);

/**
 * @brief Checks if type of value can be used where type target is expected
 *
 * @param target which type is expected.
 * @param value type of value.
 *
 * @return true if target is DTYPE_ANY or type of value and type of target are
 * equal or target is DOUBLE and value is INT,
 * false if value is UNKNOWN or BOOLEAN, default return value is false.
 */
bool is_type_compatible(DType target, DType value);

/**
 * @brief Verifies if variable exists and is mutable and the assignment value
 * has a compatible type. (x=..)
 *
 * @param symtable table to search the variable in.
 * @param name identifier for variable on the left side.
 * @param rhsType type of right hand side
 *
 * @return UNDEF_SEM_ERR if variable doesn't exist or it is a function.
 *  VAR_SEM_ERR if variable is not mutable.
 *  TYPE_INC_SEM_ERR if rhsType is not compatible with variable's type.
 *  SUCCESS if assignment is allowed.
 */
int check_assignment(SymTable *symtable, const char *name, DType rhsType);

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

/**
 * @brief Checks definition of variable and inserts it to the current scope.
 *
 * @param symtable table to be inserted in
 * @param name name of the variable
 * @param annotatedType type in the definition or DTYPE_UNKNOWN if no type is
 * given
 * @param initType type of expression on right side
 * @param isMutable true for var and false for val
 *
 * @return TYPE_INC_SEM_ERR if type is not compatible with annotatedType or
 * initType is UNKNOWN or BOOLEAN
 * REDEF_SEM_ERR if variable in this scope with this name already exists
 * SUCCESS if variable was inserted
 */
int check_variable_def(SymTable *symtable, const char *name,
                       DType annotatedType, DType initType, bool isMutable);
/**
 * @brief Checks call of function and returns its return type
 *
 * @param symtable table to search the function in.
 * @param funcName name of the function to search for
 * @param argCount number of arguments
 * @param argTypes array of argument types
 * @param outReturnType return type of function
 *
 * @return UNDEF_SEM_ERR function does not exist or it is a variable
 * PARAM_SEM_ERR if argCount is not same as symbol paramCount or argument type
 * is not compatible with the parameter type
 * SUCCESS if call is valid and *outReturnType is set
 */
int check_function_call(SymTable *symtable, const char *funcName,
                        size_t argCount, const DType *argTypes,
                        DType *outReturnType);
#endif // SEMANTIC_H
