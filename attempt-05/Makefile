CFLAGS := -Wall -Wextra -Wpedantic -g \
	$(shell pkg-config --cflags glib-2.0 gobject-2.0)
LFLAGS := $(shell pkg-config --libs glib-2.0 gobject-2.0)

CC := cc


car.o: car.c car.h
	$(CC) $(CFLAGS) $< -c -o $@

ecar.o: ecar.c ecar.h car.h
	$(CC) $(CFLAGS) $< -c -o $@

main.o: main.c ecar.h car.h
	$(CC) $(CFLAGS) $< -c -o $@

main: main.o ecar.o car.o
	$(CC) $^ -o $@ $(LFLAGS)

run: main
	./main

check: main
	valgrind ./main


clean:
	rm -f main *.o