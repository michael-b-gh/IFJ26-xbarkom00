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
 * @param p variable to check if the conditions for the error are met
 * @param length variable to store the length of the string
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

          printf("DEFVAR LF@%%p\n", stdout);
          printf("MOVE LF@%%p bool@false\n", stdout);
          printf("DEFVAR LF@%%length\n", stdout);
          printf("STRLEN LF@%%length LF@%%value\n", stdout);

          printf("LT LF@%%p LF@%%start int@0\n", stdout);
          printf("LT LF@%%p LF@%%end int@0\n", stdout);
          printf("GT LF@%%p LF@%%start LF@%%end\n", stdout);
          printf("GT LF@%%p LF@%%start LF@%%length\n", stdout);
          printf("EQ LF@%%p LF@%%start LF@%%length\n", stdout);
          printf("GT LF@%%p LF@%%end LF@%%length\n", stdout);
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n", stdout);

          printf("LABEL $subStr_loop\n", stdout);
          printf("GETCHAR LF@%%char LF@%%value LF@%%start\n", stdout);
          printf("CONCAT LF@%%substring LF@%%substring LF@%%char\n", stdout);
          printf("ADD LF@%%start LF@%%start int@1\n", stdout);
          printf("JUMPIFNEQ $subStr_loop LF@%%start LF@%%end\n", stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);

          printf("LABEL $subStr_error\n", stdout);
          printf("DPRINT int@40\n", stdout);
}
/**
 * @brief generates built-in function ord(value: String, index: Int): Int
 *
 * @param value string to get the ASCII value from
 * @param index index of the character to get the ASCII value from
 * @param char variable to store the character at the current index
 * @param length variable to store the length of the string
 * @param p variable to check if the conditions for the error are met
 * @return void
 */
void generate_ord(char *value, char *index) {
          printf("LABEL $ord\n", stdout);
          printf("CREATEFRAME\n", stdout);
          printf("PUSHFRAME\n", stdout);
          printf("DEFVAR LF@%%value\n", stdout);
          printf("DEFVAR LF@%%index\n", stdout);
          printf("DEFVAR LF@%%char\n", stdout);
          printf("DEFVAR LF@%%length\n", stdout);
          printf("DEFVAR LF@%%p\n", stdout);
          printf("MOVE LF@%%value %s\n", value, stdout);
          printf("MOVE LF@%%index %s\n", index, stdout);
          printf("STRLEN LF@%%length LF@%%value\n", stdout);

          printf("EQ LF@%%p LF@%%length int@0\n", stdout);
          printf("LT LF@%%p LF@%%index int@0\n", stdout);
          printf("EQ LF@%%p LF@%%index LF@%%length\n", stdout);
          printf("GT LF@%%p LF@%%index LF@%%length\n", stdout);
          printf("JUMPIFEQ $ord_error LF@%%p bool@true\n", stdout);

          printf("STRI2INT LF@%%char LF@%%value LF@%%index\n", stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);

          printf("LABEL $ord_error\n", stdout);
          printf("DPRINT int@-1\n", stdout);
}
/**
 * @brief generates built-in function chr(code: Int): String
 *
 * @param code ASCII code of the character to create
 * @param char variable to store the created character
 * @return void
 */
void generate_chr(char *code) {
          printf("LABEL $chr\n", stdout);
          printf("CREATEFRAME\n", stdout);
          printf("PUSHFRAME\n", stdout);
          printf("DEFVAR LF@%%char\n", stdout);
          printf("INT2STR LF@%%char %s\n", code, stdout);
          printf("POPFRAME\n", stdout);
          printf("RETURN", stdout);
}
