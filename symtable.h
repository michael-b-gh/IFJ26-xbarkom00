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

typedef enum {
  DTYPE_UNKNOWN,
  DTYPE_INT,
  DTYPE_DOUBLE,
  DTYPE_STRING,
  DTYPE_UNIT,
  DTYPE_BOOLEAN,
  DTYPE_ANY
} DType;

typedef enum { SYM_FUNCTION, SYM_VARIABLE } SymbolKind;

struct Symbol {
  bool isMain;
  size_t paramCount;
  DType returnType;
  DType *paramTypes;
  bool isDefined;
  SymbolKind kind;
  DType type;
  bool isMutable;
};

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
SymTable *symTableCreate(void);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
void symTablePushScope(SymTable *symtable);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
void symTablePopScope(SymTable *symtable);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
Symbol *symTableInsertVariable(SymTable *symtable, const char *name, DType type,
                               bool isMutable);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
Symbol *symTableDefineFunction(SymTable *symtable, const char *name,
                               size_t paramCount, const DType *paramTypes,
                               DType returnType, bool isMain);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
Symbol *symTableSearch(const SymTable *symtable, const char *name);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
void symTableDebugPrint(SymTable *symtable);

/**
 * @brief Frees created hashtable, array and copies of the keys. Does not free
 * value.
 *
 * @param table Table to be freed.
 */
void symTableFree(SymTable *symtable);

#endif
