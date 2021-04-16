CC := cc
CFLAGS := -g -Wall -Wextra -pedantic -std=gnu11


tools.o: tools.c tools.h
	$(CC) $(CFLAGS) -c $< -o $@

object.o: object.c object.h tools.o
	$(CC) $(CFLAGS) -c $< -o $@

car.o: car.c car.h tools.o
	$(CC) $(CFLAGS) -c $< -o $@

ecar.o: ecar.c ecar.h car.o
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c object.o
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o ecar.o car.o object.o tools.o
	$(CC) $^ -o $@

run: main
	@./main

check: main
	valgrind ./main


clean:
	rm -f *.o main