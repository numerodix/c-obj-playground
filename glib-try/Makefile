CFLAGS := -Wall -Wextra -Wpedantic -g \
	$(shell pkg-config --cflags glib-2.0 gobject-2.0 gmodule-2.0)
LFLAGS := $(shell pkg-config --libs glib-2.0 gobject-2.0 gmodule-2.0)

CC := cc


car.o: car.c car.h
	$(CC) $(CFLAGS) $< -c -o $@

main.o: main.c car.o
	$(CC) $(CFLAGS) $< -c -o $@

main: main.o car.o
	$(CC) $^ -o $@ $(LFLAGS)

run: main
	./main


clean:
	rm -f main *.o