/**
 * Project: IFJ26 Compiler
 *
 * @file codegen.c
 * @brief Generates code for the IFJ26 language
 * @author Lukáš Bela (xbelalu00)
 */

#include <stdio.h>
#include "codegen.h"

/**
 * @brief generates the start of the program
 *
 * @param main_name name of the main function
 * @return void
 */
void generate_program_start(const char *main_name) {
          printf(".IFJcode26\n");
          printf("CREATEFRAME\n");
          printf("DEFVAR GF@%%tmp1\n");
          printf("DEFVAR GF@%%tmp2\n");
          printf("CALL $%s\n", main_name);
          printf("EXIT int@0\n");
}

/**
 * @brief generates the end of the program
 *
 * @return void
 */
void generate_program_end() {
          generate_buildins();
}

/**
 * @brief generates the start of a function
 *
 * @param name name of the function
 * @return void
 */
void generate_func_start(const char *name) {
          printf("LABEL $%s\n", name);
          printf("PUSHFRAME\n");
}

/**
 * @brief generates the end of a function
 *
 * @return void
 */
void generate_func_end() {
          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates instruction READ
 *
 * @param var variable where the value will be saved into
 * @param type type of the value to be read
 * @return void
 */
void generate_read(char *var, char *type) {
          printf("READ %s %s\n", var, type);
}

/**
 * @brief generates instruction WRITE
 *
 * @param symb value to print
 * @return void
 */
void generate_write(char *symb) {
          printf("WRITE %s\n", symb);
}

/**
 * @brief generates instruction TYPE
 *
 * @param var output of the type of the variable
 * @param symb variable to get the type of
 * @return void
 */
void generate_type(char *var, char *symb) {
          printf("TYPE %s %s\n", var, symb);
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
          printf("ADD %s %s %s\n", var, symb1, symb2);
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
          printf("SUB %s %s %s\n", var, symb1, symb2);
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
          printf("MUL %s %s %s\n", var, symb1, symb2);
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
          printf("DIV %s %s %s\n", var, symb1, symb2);
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
          printf("IDIV %s %s %s\n", var, symb1, symb2);
}

/**
 * @brief generates instruction PUSHS
 *
 * @param symb variable to push onto the stack
 * @return void
 */
void generate_pushs(char *symb) {
          printf("PUSHS %s\n", symb);
}

/**
 * @brief generates instruction POPS
 *
 * @param var variable to save the popped value into
 * @return void
 */
void generate_pops(char *var) {
          printf("POPS %s\n", var);
}

/**
 * @brief generates instruction CLEARS
 *
 * @return void
 */
void generate_clears() {
          printf("CLEARS\n");
}

/**
 * @brief generates stack instruction ADDS
 *
 * @return void
 */
void generate_adds() {
          printf("ADDS\n");
}

/**
 * @brief generates stack instruction SUBS
 *
 * @return void
 */
void generate_subs() {
          printf("SUBS\n");
}

/**
 * @brief generates stack instruction MULS
 *
 * @return void
 */
void generate_muls() {
          printf("MULS\n");
}

/**
 * @brief generates stack instruction DIVS
 *
 * @return void
 */
void generate_divs() {
          printf("DIVS\n");
}

/**
 * @brief generates stack instruction IDIVS
 *
 * @return void
 */
void generate_idivs() {
          printf("IDIVS\n");
}

/**
 * @brief generates stack instruction LTS
 *
 * @return void
 */
void generate_lts() {
          printf("LTS\n");
}

/**
 * @brief generates stack instruction GTS
 *
 * @return void
 */
void generate_gts() {
          printf("GTS\n");
}

/**
 * @brief generates stack instruction EQS
 *
 * @return void
 */
void generate_eqs() {
          printf("EQS\n");
}

/**
 * @brief generates stack instruction NOTS
 *
 * @return void
 */
void generate_nots() {
          printf("NOTS\n");
}

/**
 * @brief generates stack instruction INT2FLOATS
 *
 * @return void
 */
void generate_int2floats() {
          printf("INT2FLOATS\n");
}

/**
 * @brief generates instruction CONCATS
 *
 * @param var variable where the concatenated string will be saved into
 * @param tmp1 first string to concatenate
 * @param tmp2 second string to concatenate
 * @return void
 */
void generate_concats() {
          printf("POPS GF@%%tmp2\n");
          printf("POPS GF@%%tmp1\n");
          printf("CONCAT GF@%%tmp1 GF@%%tmp1 GF@%%tmp2\n");
          printf("PUSHS GF@%%tmp1\n");
}

/**
 * @brief generates instruction ASSIGN
 *
 * @param varName name of the variable to assign the value to
 * @return void
 */
void generate_assign(const char *varName){
          printf("POPS LF@%s\n", varName);
}

/**
 * @brief generates built-in function readInt(): Int
 *
 * @param var variable where the value will be saved into
 * @return void
 */
void generate_readInt() {
          printf("LABEL $readInt\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%var\n");
          printf("READ LF@%%var int\n");

          printf("PUSHS LF@%%var\n");

          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function readDouble(): Double
 *
 * @param var variable where the value will be saved into
 * @return void
 */
void generate_readDouble() {
          printf("LABEL $readDouble\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%var\n");
          printf("READ LF@%%var float\n");

          printf("PUSHS LF@%%var\n");

          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function readLine(): String
 *
 * @param var variable where the value will be saved into
 * @return void
 */
void generate_readLine() {
          printf("LABEL $readLine\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%var\n");
          printf("READ LF@%%var string\n");

          printf("PUSHS LF@%%var\n");

          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function print(value: Any): Unit
 *
 * @param  value value to print
 * @return void
 */
void generate_print() {
          printf("LABEL $print\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");

          printf("DEFVAR LF@%%value\n");
          printf("POPS LF@%%value\n");
          printf("WRITE LF@%%value\n");

          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function toStr(value: Any): String
 *
 * @param value value to convert to string
 * @param type variable to store the type of the value
 * @param str variable to store the string representation of the value
 * @return void
 */
void generate_toStr() {
          printf("LABEL $toStr\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%value\n");
          printf("DEFVAR LF@%%type\n");
          printf("DEFVAR LF@%%str\n");
          printf("POPS LF@%%value\n");
          
          printf("TYPE LF@%%type LF@%%value\n");
          printf("JUMPIFEQ $toStr_int LF@%%type string@int\n");
          printf("JUMPIFEQ $toStr_float LF@%%type string@float\n");
          printf("JUMPIFEQ $toStr_bool LF@%%type string@bool\n");
          printf("JUMPIFEQ $toStr_nil LF@%%type string@nil\n");
          printf("JUMPIFEQ $toStr_string LF@%%type string@string\n");

          printf("LABEL $toStr_int\n");
          printf("INT2STR LF@%%str LF@%%value\n");
          printf("JUMP $toStr_end\n");

          printf("LABEL $toStr_float\n");
          printf("FLOAT2STR LF@%%str LF@%%value\n");
          printf("JUMP $toStr_end\n");
          
          printf("LABEL $toStr_bool\n");
          printf("JUMPIFEQ $toStr_true LF@%%value bool@true\n");
          printf("MOVE LF@%%str string@false\n");
          printf("JUMP $toStr_end\n");
          printf("LABEL $toStr_true\n");
          printf("MOVE LF@%%str string@true\n");
          printf("JUMP $toStr_end\n");

          printf("LABEL $toStr_nil\n");
          printf("MOVE LF@%%str string@nil\n");
          printf("JUMP $toStr_end\n");

          printf("LABEL $toStr_string\n");
          printf("MOVE LF@%%str LF@%%value\n");
          printf("JUMP $toStr_end\n");

          printf("LABEL $toStr_end\n");
          printf("PUSHS LF@%%str\n");
          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function toInt(value: Double): Int
 *
 * @param value value to convert to int
 * @param int variable to store the int representation of the value
 * @return void
 */
void generate_toInt() {
          printf("LABEL $toInt\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%value\n");
          printf("DEFVAR LF@%%int\n");
          printf("POPS LF@%%value\n");

          printf("FLOAT2INT LF@%%int LF@%%value\n");

          printf("PUSHS LF@%%int\n");
          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function strLenght(value: String): Int
 *
 * @param value string to get the length of
 * @param length variable to store the length of the string
 * @return void
 */
void generate_strLenght() {
          printf("LABEL $strLenght\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%value\n");
          printf("DEFVAR LF@%%length\n");
          printf("POPS LF@%%value\n");

          printf("STRLEN LF@%%length LF@%%value\n");

          printf("PUSHS LF@%%length\n");
          printf("POPFRAME\n");
          printf("RETURN\n");
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
void generate_subStr() {
          printf("LABEL $subStr\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%value\n");
          printf("DEFVAR LF@%%start\n");
          printf("DEFVAR LF@%%end\n");
          printf("DEFVAR LF@%%char\n");
          printf("DEFVAR LF@%%substring\n");
          printf("MOVE LF@%%substring string@\n");

          printf("POPS LF@%%end\n");
          printf("POPS LF@%%start\n");
          printf("POPS LF@%%value\n");

          printf("DEFVAR LF@%%p\n");
          printf("MOVE LF@%%p bool@false\n");
          printf("DEFVAR LF@%%length\n");
          printf("STRLEN LF@%%length LF@%%value\n");

          printf("LT LF@%%p LF@%%start int@0\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");
          printf("LT LF@%%p LF@%%end int@0\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");
          printf("GT LF@%%p LF@%%start LF@%%end\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");
          printf("GT LF@%%p LF@%%start LF@%%length\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");
          printf("EQ LF@%%p LF@%%start LF@%%length\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");
          printf("GT LF@%%p LF@%%end LF@%%length\n");
          printf("JUMPIFEQ $subStr_error LF@%%p bool@true\n");

          printf("LABEL $subStr_loop\n");
          printf("JUMPIFEQ $subStr_end LF@%%start LF@%%end\n");
          printf("GETCHAR LF@%%char LF@%%value LF@%%start\n");
          printf("CONCAT LF@%%substring LF@%%substring LF@%%char\n");
          printf("ADD LF@%%start LF@%%start int@1\n");
          printf("JUMP $subStr_loop\n");

          printf("LABEL $subStr_end\n");
          printf("PUSHS LF@%%substring\n");

          printf("POPFRAME\n");
          printf("RETURN\n");

          printf("LABEL $subStr_error\n");
          printf("EXIT int@40\n");
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
void generate_ord() {
          printf("LABEL $ord\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%value\n");
          printf("DEFVAR LF@%%index\n");
          printf("DEFVAR LF@%%char\n");
          printf("DEFVAR LF@%%length\n");
          printf("DEFVAR LF@%%p\n");
          
          printf("POPS LF@%%index\n");
          printf("POPS LF@%%value\n");

          printf("STRLEN LF@%%length LF@%%value\n");

          printf("EQ LF@%%p LF@%%length int@0\n");
          printf("JUMPIFEQ $ord_error LF@%%p bool@true\n");
          printf("LT LF@%%p LF@%%index int@0\n");
          printf("JUMPIFEQ $ord_error LF@%%p bool@true\n");
          printf("EQ LF@%%p LF@%%index LF@%%length\n");
          printf("JUMPIFEQ $ord_error LF@%%p bool@true\n");
          printf("GT LF@%%p LF@%%index LF@%%length\n");
          printf("JUMPIFEQ $ord_error LF@%%p bool@true\n");

          printf("STRI2INT LF@%%char LF@%%value LF@%%index\n");

          printf("PUSHS LF@%%char\n");

          printf("POPFRAME\n");
          printf("RETURN\n");

          printf("LABEL $ord_error\n");
          printf("PUSHS int@-1\n");
          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates built-in function chr(code: Int): String
 *
 * @param code ASCII code of the character to create
 * @param char variable to store the created character
 * @return void
 */
void generate_chr() {
          printf("LABEL $chr\n");
          printf("CREATEFRAME\n");
          printf("PUSHFRAME\n");
          printf("DEFVAR LF@%%char\n");
          printf("DEFVAR LF@%%code\n");

          printf("POPS LF@%%code\n");

          printf("INT2CHAR LF@%%char LF@%%code\n");

          printf("PUSHS LF@%%char\n");
          printf("POPFRAME\n");
          printf("RETURN\n");
}

/**
 * @brief generates all built-in functions
 *
 * @return void
 */
void generate_buildins() {
          generate_readInt();
          generate_readDouble();
          generate_readLine();
          generate_print();
          generate_toStr();
          generate_toInt();
          generate_strLenght();
          generate_subStr();
          generate_ord();
          generate_chr();
}
