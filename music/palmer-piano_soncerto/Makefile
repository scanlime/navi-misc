all:	cello.pdf piano.pdf

cello.pdf:	cello.ly 01_allegro_ma_non_troppo/cello.ly 01_allegro_ma_non_troppo/text.ly
	lilypond cello.ly

piano.pdf:  piano.ly 01_allegro_ma_non_troppo/piano.ly
	lilypond piano.ly

clean:
	rm -f *.{log,tex,dvi,ps}
