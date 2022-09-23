CC = gcc
CFLAGS = -std=c11 -g -O2 -Wall -Wextra -Werror -pedantic

.PHONY: all run clean zip

all: main

main: main.c
	$(CC) $(CFLAGS) $^ -o $@

run: main
	./main

clean:
	rm -f *.o *.out xkucha28.zip

zip:
	zip xkucha28.zip *.c *.h Makefile rozdeleni
