all: bassoon.pdf cello.pdf flute.pdf piano.pdf score.pdf

bassoon.pdf: bassoon.ly 01_allegro_ma_non_troppo/bassoon.ly
	lilypond bassoon.ly

cello.pdf: cello.ly 01_allegro_ma_non_troppo/cello.ly 01_allegro_ma_non_troppo/text.ly
	lilypond cello.ly

flute.pdf: flute.ly 01_allegro_ma_non_troppo/flute.ly
	lilypond flute.ly

piano.pdf: piano.ly 01_allegro_ma_non_troppo/piano.ly
	lilypond piano.ly

score.pdf: 01_allegro_ma_non_troppo/bassoon.ly 01_allegro_ma_non_troppo/cello.ly 01_allegro_ma_non_troppo/piano.ly 01_allegro_ma_non_troppo/text.ly
	lilypond score.ly

clean:
	rm -f *.{log,tex,dvi,ps}
