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


/**
 * @brief generates built-in function readInt(): Int
 *
 * @param LF@%var variable where the value will be saved into
 * @return void
 */
void generate_readInt() {
          printf("LABEL $readInt\n", stdout);
          printf("CREATEFRAME\n", stdout);
          printf("PUSHFRAME\n", stdout);
          printf("DEFVAR LF@%%var\n", stdout);
          printf("READ LF@%%var int\n", stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);
}
/**
 * @brief generates built-in function print(value: Any): Unit
 *
 * @param  value value to print
 * @return void
 */
void generate_print(char *value) {
          printf("LABEL $print\n", stdout);
          printf("CREATEFRAME\n", stdout);
          printf("PUSHFRAME\n", stdout);
          printf("WRITE %s\n", value, stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);
}
/**
 * @brief generates built-in function subStr(value: String, start: Int, end: Int): String
 *
 * @param value value to get the substring from
 * @param start starting index of the substring 
 * @param end ending index of the substring
 * @param char variable to store the character at the current index
 * @param substring variable where the substring will be saved into
 * @return void
 */
void generate_subStr(char *value, char *start, char *end) {
          printf("LABEL $subStr\n", stdout);
          printf("CREATEFRAME\n", stdout);
          printf("PUSHFRAME\n", stdout);
          printf("DEFVAR LF@%%value\n", stdout);
          printf("DEFVAR LF@%%start\n", stdout);
          printf("DEFVAR LF@%%end\n", stdout);
          printf("DEFVAR LF@%%char\n", stdout);
          printf("DEFVAR LF@%%substring\n", stdout);
          printf("MOVE LF@%%value %s\n", value, stdout);
          printf("MOVE LF@%%start %s\n", start, stdout);
          printf("MOVE LF@%%end %s\n", end, stdout);

        //TODO: Implement conditions for start and end index validation
          printf("LABEL $subStr_loop\n", stdout);
          printf("GETCHAR LF@%%char LF@%%value LF@%%start\n", stdout);
          printf("CONCAT LF@%%substring LF@%%substring LF@%%char\n", stdout);
          printf("ADD LF@%%start LF@%%start int@1\n", stdout);
          printf("JUMPIFNEQ $subStr_loop LF@%%start LF@%%end\n", stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);
}
