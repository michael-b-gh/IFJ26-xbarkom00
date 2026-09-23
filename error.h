/**
* Project: IFJ26 Compiler
*
* @file error.h
* @brief definitions of errors
* @author Mykhailo Barkovskyi (xbarkom00)
*/

#ifndef _ERROR_IFJ26_H
#define _ERROR_IFJ26_H

#define SUCCESS 0 // No errors
#define LEX_ERR 1 // Lexical error
#define SYN_ERR 2 // Syntax error
#define UNDEF_SEM_ERR 3 // Undefined function/variable/type
#define REDEF_SEM_ERR 4 // Redefenition of function/variable/type
#define PARAM_SEM_ERR 5 // Wrong parameters
#define TYPE_INC_SEM_ERR 6 // Type incompatibility
#define VAR_SEM_ERR 7 // Writing to immutable variable
#define OTHER_SEM_ERR 10  // Other semantic errors
#define INT_ERR 99 // Internal compiler error (malloc failure) 

#endif
