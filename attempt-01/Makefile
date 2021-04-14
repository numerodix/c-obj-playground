#CFLAGS := -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS := -Wall -Wextra -pedantic -std=c11


tools.o: tools.c tools.h
	cc $(CFLAGS) -c $< -o $@

object.o: object.c object.h tools.o
	cc $(CFLAGS) -c $< -o $@

main.o: main.c object.o
	cc $(CFLAGS) -c $< -o $@

main: main.o object.o tools.o
	cc $^ -o $@

run: main
	@./main


clean:
	rm -f *.o main