CC     = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0)
LIBS   = $(shell pkg-config --libs gtk+-3.0) -lm
SRC := $(wildcard sources/*.c)

simplex: main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o simplex $(LIBS)

run: simplex
	./simplex

clean:
	rm -rf simplex salida
	rm -rf simplex_test

temp:
	$(CC) $(CFLAGS) $(SRC) -o simplex_temp -lm
	./simplex_temp
	pdflatex -output-directory=results tex_files/main.tex
