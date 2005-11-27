#
# Shared Makefile rules for all documents. See the template
# for info on variables this will look for.
#

all: $(DOCNAME).pdf

CONVERTED_SVG_FIGURES = $(SVG_FIGURES:.svg=.pdf)
SVG_TEMP_POSTSCRIPT = $(SVG_FIGURES:.svg=.ps)

ALL_FIGURES = \
	$(CONVERTED_SVG_FIGURES)	\
	$(STATIC_FIGURES)		\
	$(GENERATED_FIGURES)

# Extra search paths for latex
TEXINPUTS = .:$(LIB_DIR)
BIBINPUTS = .:$(LIB_DIR)

# Note that we run pdflatex twice, so cross-references
# and the table of contents are up to date.
$(DOCNAME).pdf: *.tex $(ALL_FIGURES) #$(LIB_DIR)/*.tex
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $(DOCNAME).tex
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $(DOCNAME).tex

bib: *.tex
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $(DOCNAME).tex
	BIBINPUTS=$(BIBINPUTS):$$BIBINPUTS bibtex $(DOCNAME)
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $(DOCNAME).tex
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $(DOCNAME).tex

%.pdf: %.ps
	epstopdf $<

%.ps: %.svg
	inkscape -z -f $< -p ">$@"

ALL_TEMPFILES = \
	*.log *.toc *.lof *.aux *.dvi *.glo *.out \
	$(CONVERTED_SVG_FIGURES)	\
	$(GENERATED_FIGURES)		\
	$(SVG_TEMP_POSTSCRIPT)

spellcheck: $(DOCNAME).tex
	aspell -t check $<

# Clean everything except the final output file
clean-tempfiles:
	rm -rf $(ALL_TEMPFILES)

clean: clean-tempfiles
	rm -rf $(DOCUMENTS)

