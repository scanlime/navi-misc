#
# Shared Makefile rules for all documents. See the template
# for info on variables this will look for.
#

all: $(DOCUMENTS)

CONVERTED_SVG_FIGURES = $(SVG_FIGURES:.svg=.pdf)
SVG_TEMP_POSTSCRIPT = $(SVG_FIGURES:.svg=.ps)

ALL_FIGURES = \
	$(CONVERTED_SVG_FIGURES)	\
	$(STATIC_FIGURES)		\
	$(GENERATED_FIGURES)

# Extra search paths for latex
TEXINPUTS = .:$(LIB_DIR)

# Note that we run pdflatex twice, so cross-references
# and the table of contents are up to date.
%.pdf: %.tex $(ALL_FIGURES)
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $<
	TEXINPUTS=$(TEXINPUTS):$$TEXINPUTS pdflatex $<

%.pdf: %.ps
	epstopdf $<

%.ps: %.svg
	inkscape -z -f $< -p ">$@"

ALL_TEMPFILES = \
	*.log *.toc *.lof *.aux *.dvi *.glo *.out \
	$(CONVERTED_SVG_FIGURES)	\
	$(GENERATED_FIGURES)		\
	$(SVG_TEMP_POSTSCRIPT)

# Clean everything except the final output file
clean-tempfiles:
	rm -rf $(ALL_TEMPFILES)

clean: clean-tempfiles
	rm -rf $(DOCUMENTS)

