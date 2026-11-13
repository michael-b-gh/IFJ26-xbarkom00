/**
 * Project: IFJ26 Compiler
 *
 * @file codegen.h
 * @brief Declarations of code generator functions for IFJcode26
 * @author Lukáš Bela (xbelalu00)
 */

#ifndef _CODEGEN_IFJ26_H
#define _CODEGEN_IFJ26_H

void generate_read(char *var, char *type);

void generate_write(char *symb);

void generate_type(char *var, char *symb);

void generate_add(char *var, char *symb1, char *symb2);

void generate_sub(char *var, char *symb1, char *symb2);

void generate_mul(char *var, char *symb1, char *symb2);

void generate_div(char *var, char *symb1, char *symb2);

void generate_idiv(char *var, char *symb1, char *symb2);

void generate_readInt();

void generate_print(char *value);

void generate_subStr(char *value, char *start, char *end);

void generate_ord(char *value, char *index);

void generate_chr(char *code);

#endif