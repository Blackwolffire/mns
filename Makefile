CC=gcc
CFLAGS= -pedantic -Wall -Wextra -Werror -std=c99 -Iinc -g
SRC= minishell.c ast.c lexer.c parser.c executer.c builtin.c tools.c
OBJ= $(SRC:.c=.o)
BIN= minishell
VPATH= src

.PHONY: clean

all: $(BIN)

$(BIN): $(OBJ)

clean:
	$(RM) $(OBJ) $(BIN)
