SOURCE_DIR = sources

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)

output:
	gcc `pkg-config --cflags gtk+-3.0` $(SOURCES) -o simplex -lm  `pkg-config --libs gtk+-3.0`

tex:
	pdflatex -output-directory=results tex_files/main.tex