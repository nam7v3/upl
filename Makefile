CC = gcc
CFLAGS = -O2 -std=gnu11
all: upl-lexer

upl-lexer: src/lexer-flex.o src/lexer-handmade.o src/main.o
	$(CC) -o upl-lexer $^

src/%.o: src/%.c

src/lexer-flex.c src/lexer-flex.h: src/lexer-flex.l
	flex --header-file=src/lexer-flex.h -o src/lexer-flex.c $^

.PHONY: clean
clean:
	rm -rf src/*.o upl-lexer src/lexer-flex.c src/lexer-flex.h
