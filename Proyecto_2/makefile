CC     = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0)
LIBS   = $(shell pkg-config --libs gtk+-3.0) -lm
SRC := $(wildcard sources/*.c)

simplex: main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o simplex $(LIBS)

run: simplex
	./simplex

clean:
	rm -f pdflatex_output/*
	rm -f simplex
