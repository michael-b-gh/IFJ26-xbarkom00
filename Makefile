#
# Project: IFJ26 Compiler
#
# Makefile compiles all .c files into an executable ifj26
# Author: Mykhailo Barkovskyi
# Author: Lukáš Zeleňák
#


CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
SRCS = Lexer.c token_buffer.c parser.c expression.c semantic.c symtable.c hashtable.c codegen.c
TARGET = ifj26
.PHONY: build check build-check clean test

build:
		$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

check:
		$(CC) $(CFLAGS) -Werror $(SRCS) -fsyntax-only

build-check: check build

clean:
		rm -f $(TARGET)

test:
		bash tests/run_tests.sh