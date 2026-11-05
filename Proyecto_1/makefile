CC     = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0)
LIBS   = $(shell pkg-config --libs gtk+-3.0) -lm

simplex: main.c
	$(CC) $(CFLAGS) main.c -o simplex $(LIBS)

run: simplex
	./simplex

clean:
	rm -rf simplex salida