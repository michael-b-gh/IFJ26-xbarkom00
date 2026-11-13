/**
 * Project: IFJ26 Compiler
 *
 * @file symtable.h
 * @brief Symbol table with nested scopes
 * @author Lukas Zelenak (xzelenl00)
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Symbol Symbol;
typedef struct SymTable SymTable;

/**
 * @brief Data type of symbol.
 */
typedef enum {
  DTYPE_UNKNOWN, /**< If type is unknown, default type. */
  DTYPE_INT,     /**< Type is Int. */
  DTYPE_DOUBLE,  /**< Type is Double. */
  DTYPE_STRING,  /**< Type is String. */
  DTYPE_UNIT,    /**< Type is Unit, without returning value. */
  DTYPE_BOOLEAN, /**< Type is bool */
  DTYPE_ANY /**< Type is Any, used only with params in built-in functions. */
} DType;

/**
 * @brief Kind of symbol.
 */
typedef enum {
  SYM_FUNCTION, /**< Symbol is function. */
  SYM_VARIABLE  /**< Symbol is variable. */
} SymbolKind;

/**
 * @brief Symbol information
 */
struct Symbol {
  bool isMain;       /**< Is true if this is \@main function. (function only) */
  size_t paramCount; /**< Number of params. (function only) */
  DType returnType;  /**< Return type of the function. (function only) */
  DType *paramTypes; /**< Types of params, can be NULL if paramCount is 0.
                        (function only) */
  bool isDefined;    /**< Is true if function has body. (function only) */
  SymbolKind kind;   /**< Kind of the symbol */
  DType type;        /**< Type of the symbol. */
  bool isMutable;    /**< Is true if value can be changed. (variable only) */
};

/**
 * @brief Allocates empty symtable with one opened global scope. On memory alloc
 * failure program exits with INT_ERR. Built-in functions are already defined in
 * the global scope.
 *
 * @return Pointer to new symtable. Must be freed with symTableFree
 */
SymTable *symTableCreate(void);

/**
 * @brief Creates new empty scope which will be current. On memory alloc
 * failure program exits with INT_ERR.
 *
 * @param symtable Table to push scope in.
 */
void symTablePushScope(SymTable *symtable);

/**
 * @brief Frees all of the symbols in the current scope and closes it except
 * global scope (INT_ERR). Pointers to symbols are no longer valid.
 *
 * @param symtable Table to pop scope from.
 */
void symTablePopScope(SymTable *symtable);

/**
 * @brief Defines variable symbol, inserts to current scope. On memory alloc
 * failure program exits with INT_ERR.
 *
 * @param symtable Table to define the variable in.
 * @param name Name of the variable.
 * @param type Type of the variable.
 * @param isMutable Is true if variable value can be changed.
 *
 * @return Symbol If variable is successfully inserted.
 *         NULL If variable in the current scope with the name already exists.
 *         NULL If symtable is NULL.
 */
Symbol *symTableInsertVariable(SymTable *symtable, const char *name, DType type,
                               bool isMutable);

/**
 * @brief Defines function symbol, inserts to global scope and copies
 * paramTypes. On memory alloc failure program exits with INT_ERR.
 *
 * @param symtable Table to define the function in.
 * @param name Name of the function.
 * @param paramCount Number of params of the function.
 * @param paramTypes Types of params, can be NULL if paramCount is 0.
 * @param returnType Return type of the function.
 * @param isMain Is true if this is @main function.
 *
 * @return Symbol If function is successfully inserted.
 *         NULL If function with the name already exists.
 *         NULL If symtable is NULL.
 */
Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
                               size_t paramCount, const DType *paramTypes,
                               DType returnType, bool isMain);

/**
 * @brief Searches the symbol in all scopes (current to global), for item with
 * the given name.
 *
 * @param symtable Table to search in.
 * @param name Name of the symbol to search for.
 *
 * @return Symbol If it is found.
 *         NULL If it is not found.
 *         NULL If symtable is NULL.
 */
Symbol *symTableSearch(const SymTable *symtable, const char *name);

/**
 * @brief Counts functions marked with \@main in the global scope.
 *
 * @param symtable Table to count in.
 *
 * @return Number of main functions, 0 if symtable is NULL.
 */
size_t symTableMainCount(const SymTable *symtable);

/**
 * @brief Frees created symtable and all symbols in the scopes. If NULL nothing
 * will be freed.
 *
 * @param symtable Table to be freed.
 */
void symTableFree(SymTable *symtable);

#endif
