#ITY - Projekt 4
#Autor: Simona Jánošíková

NAME = proj4

.PHONY: export_pdf clean

all: export_pdf

export_pdf:$(NAME).tex
	latex $(NAME).tex
	bibtex $(NAME).aux
	latex $(NAME).tex
	latex $(NAME).tex
	dvips  -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f $(NAME).out $(NAME).ps $(NAME).aux $(NAME).dvi $(NAME).log