CC := cc
CFLAGS := -g -Wall -Wextra -pedantic -std=gnu11


main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $^ -o $@

run: main
	@./main

check: main
	valgrind ./main


clean:
	rm -f *.o main