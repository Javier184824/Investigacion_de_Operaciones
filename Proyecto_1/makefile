SOURCE_DIR = sources

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)

output:
	gcc $(SOURCES) -o simplex -lm

tex:
	pdflatex -output-directory=results tex_files/main.tex