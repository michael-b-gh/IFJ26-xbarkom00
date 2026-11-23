/**
 * Project: IFJ26 Compiler
 *
 * @file codegen.h
 * @brief Declarations of code generator functions for IFJcode26
 * @author Lukáš Bela (xbelalu00)
 */

#ifndef _CODEGEN_IFJ26_H
#define _CODEGEN_IFJ26_H

void generate_program_start(const char *main_name);

void generate_program_end();

void generate_func_start(const char *name);

void generate_func_end();

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

void generate_adds();

void generate_subs();

void generate_muls();

void generate_divs();

void generate_idivs();

void generate_lts();

void generate_gts();

void generate_eqs();

void generate_nots();

void generate_int2floats();

void generate_concats();

void generate_assign(const char *varName);

void generate_readInt();

void generate_readDouble();

void generate_readLine();

void generate_print();

void generate_toStr();

void generate_toInt();

void generate_strLenght();

void generate_subStr();

void generate_ord();

void generate_chr();

void generate_buildins();

#endif
