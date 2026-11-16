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

void generate_pushs(char *symb);

void generate_pops(char *var);

void generate_clears();

void generate_adds(char *var, char *symb1, char *symb2);

void generate_subs(char *var, char *symb1, char *symb2);

void generate_muls(char *var, char *symb1, char *symb2);

void generate_divs(char *var, char *symb1, char *symb2);

void generate_idivs(char *var, char *symb1, char *symb2);

void generate_lts(char *var, char *symb1, char *symb2);

void generate_gts(char *var, char *symb1, char *symb2);

void generate_eqs(char *var, char *symb1, char *symb2);

void generate_nots(char *var, char *symb1);

void generate_int2floats(char *var, char *symb);

void generate_concats(char *var, char *symb1, char *symb2);

void generate_readInt();

void generate_print(char *value);

void generate_subStr(char *value, char *start, char *end);

void generate_ord(char *value, char *index);

void generate_chr(char *code);

#endif
