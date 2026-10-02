/**
 * Project: IFJ26 Compiler
 *
 * @file codegen.h
 * @brief Generates code for the IFJ26 language
 * @author Lukáš Bela (xbelalu00)
 */

#ifndef _CODEGEN_IFJ26_H
#define _CODEGEN_IFJ26_H

#include <stdio.h>

/**
 * @brief generates instruction READ
 *
 * @param var variable where the value will be saved into
 * @param type type of the value to be read
 * @return void
 */
void generate_read(char *var, char *type) {
          printf("READ %s %s\n", var, type, stdout);
}

/**
 * @brief generates instruction WRITE
 *
 * @param symb value to print
 * @return void
 */
void generate_write(char *symb) {
          printf("WRITE %s\n", symb, stdout);
}

/**
 * @brief generates instruction TYPE
 *
 * @param var output of the type of the variable
 * @param symb variable to get the type of
 * @return void
 */
void generate_type(char *var, char *symb) {
          printf("TYPE %s %s\n", var, symb, stdout);
}

/**
 * @brief generates instruction ADD
 *
 * @param var variable where the value will be saved into
 * @param symb1 first operand
 * @param symb2 second operand
 * @return void
 */
void generate_add(char *var, char *symb1, char *symb2) {
          printf("ADD %s %s %s\n", var, symb1, symb2, stdout);
}

/**
 * @brief generates instruction SUB
 *
 * @param var variable where the value will be saved into
 * @param symb1 first operand
 * @param symb2 second operand
 * @return void
 */
void generate_sub(char *var, char *symb1, char *symb2) {
          printf("SUB %s %s %s\n", var, symb1, symb2, stdout);
}

/**
 * @brief generates instruction MUL
 *
 * @param var variable where the value will be saved into
 * @param symb1 first operand
 * @param symb2 second operand
 * @return void
 */
void generate_mul(char *var, char *symb1, char *symb2) {
          printf("MUL %s %s %s\n", var, symb1, symb2, stdout);
}

/**
 * @brief generates instruction DIV
 *
 * @param var variable where the value will be saved into
 * @param symb1 first operand
 * @param symb2 second operand
 * @return void
 */
void generate_div(char *var, char *symb1, char *symb2) {
          printf("DIV %s %s %s\n", var, symb1, symb2, stdout);
}

/**
 * @brief generates instruction IDIV
 *
 * @param var variable where the value will be saved into
 * @param symb1 first operand
 * @param symb2 second operand
 * @return void
 */
void generate_idiv(char *var, char *symb1, char *symb2) {
          printf("IDIV %s %s %s\n", var, symb1, symb2, stdout);
}