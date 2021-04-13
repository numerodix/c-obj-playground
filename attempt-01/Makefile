#CFLAGS := -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS := -Wall -Wextra -pedantic -std=c11


main: main.c tools.h
	cc $(CFLAGS) $< -o $@

run: main
	@./main