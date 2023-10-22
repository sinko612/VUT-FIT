#ITY - Projekt 5
#Autor: Simona Jánošíková

NAME = proj5

.PHONY: export_pdf clean

all: export_pdf

export_pdf: $(NAME).tex
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

clean:
	rm -f $(NAME).out $(NAME).ps $(NAME).aux $(NAME).dvi $(NAME).log
