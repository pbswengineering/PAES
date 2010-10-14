.PHONY: clean all

all: tesi-specialistica.pdf

grafico-%.png: %.gp
	gnuplot $^

tesi-specialistica.pdf: tesi-specialistica.tex bibliografia.bib
	pdflatex tesi-specialistica
	pdflatex tesi-specialistica
	bibtex tesi-specialistica
	pdflatex tesi-specialistica
	./correggi-lol.py tesi-specialistica.lol
	pdflatex tesi-specialistica

clean:
	rm -f *.pdf *.dvi *.aux grafico-*.png *.toc *.out *~ *.bbl *.blg *.lo? *.tmp graph/*.ps graph/*.pdf graph/*~

