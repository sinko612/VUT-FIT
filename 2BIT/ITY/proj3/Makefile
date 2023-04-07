#ITY - Projekt 3
#Autor: Simona Jánošíková

NAME = proj3

.PHONY: export_pdf clean

all: export_pdf

export_pdf:$(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips  -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps

clean:
	rm -f $(NAME).out $(NAME).ps $(NAME).aux $(NAME).dvi $(NAME).log
