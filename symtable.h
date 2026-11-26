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
typedef struct SymTableContext SymTableContext;

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
 * @brief State of return type inference of a function
 */
typedef enum {
  INFER_UNVISITED = 0,   /**< Type wasn't inferred */
  INFER_IN_PROGRESS = 1, /**< Type is being inferred */
  INFER_RESOLVED = 2     /**< Type is known */
} InferState;

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
  bool hasExplicitReturn; /**< True if the return type is given in the header.
                             (function only) */
  size_t bodyStartIdx; /**< Index in the token buffer where the function starts.
                          (function only) */
  InferState inferState; /**< State of return type inference. (function only)*/
  size_t varId; /**< Unique ID of variable, for unique name. (variable only) */
  size_t
      firstVarId; /**< First ID of the variables in function. (function only) */
  size_t
      lastVarId; /**< Last ID of the variables in function. (function only) */
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
 * @param hasExplicitReturn Is true if return type is in function header.
 * @param bodyStartIdx Index in token buffer where function starts.
 *
 * @return Symbol If function is successfully inserted.
 *         NULL If function with the name already exists.
 *         NULL If symtable is NULL.
 */
Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
                               size_t paramCount, const DType *paramTypes,
                               DType returnType, bool isMain,
                               bool hasExplicitReturn, size_t bodyStartIdx);

/**
 * @brief Searches the symbol in all scopes (current to global), for item with
 * the given name.
 *
 * @param symtable Table to search in.
 * @param name Name of the symbol to search for.
 *
 * @return Symbol If it is found.
 *         NULL If it is not found.
 *         INT_ERR If symtable is NULL or name is NULL.
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

/**
 * @brief Saves local scopes, only the global scope stays visible.
 *
 * @param symtable Table whose local scopes are saved.
 *
 * @return Context with saved scopes. If allocation fails exits with INT_ERR.
 */
SymTableContext *symTableSaveContext(SymTable *symtable);

/**
 * @brief Restores saved scopes and frees the context. Only global scope must be
 * open otherwise program exits with INT_ERR.
 *
 * @param symtable Table to restore the scopes into.
 * @param context Context returned from symTableSaveContext
 */
void symTableRestoreContext(SymTable *symtable, SymTableContext *context);

/**
 * @brief Returns ID, which the next variable will get.
 *
 * @param symtable Table to get next variable ID.
 *
 * @return ID which the next variable will get, 0 if table is NULL.
 */
size_t symTableGetNextVarId(const SymTable *symtable);

/**
 * @brief Sets counter (nextVarId) to the value from nextId.
 *
 * @param symtable Table to set next variable ID.
 * @param nextId ID that the next variable will get when inserted.
 */
void symTableSetNextVarId(SymTable *symtable, size_t nextId);

#endif
