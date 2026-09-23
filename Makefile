#
# Project: IFJ26 Compiler
#
# Makefile compiles all .c files into an executable ifj26
# Author: Mykhailo Barkovskyi
# Login: xbarkom00
#

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic

build:
		$(CC) $(CFLAGS) *.c -o ifj26

clean: 
		rm -f ifj26
