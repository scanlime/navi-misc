all: flute.pdf bassoon.pdf piano.pdf cello.pdf
	

flute.pdf: flute.ly 01_allegro_ma_non_troppo/flute.ly
	lilypond flute.ly

bassoon.pdf: bassoon.ly 01_allegro_ma_non_troppo/bassoon.ly
	lilypond bassoon.ly

cello.pdf: cello.ly 01_allegro_ma_non_troppo/cello.ly 01_allegro_ma_non_troppo/text.ly
	lilypond cello.ly

piano.pdf: piano.ly 01_allegro_ma_non_troppo/piano.ly
	lilypond piano.ly

clean:
	rm -f *.{log,tex,dvi,ps}
