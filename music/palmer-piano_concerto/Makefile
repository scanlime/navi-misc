all: bassoon.pdf cello.pdf flute.pdf piano.pdf score.pdf

BASSOON = 01_allegro_ma_non_troppo/bassoon.ly
CELLO = 01_allegro_ma_non_troppo/cello.ly
FLUTE = 01_allegro_ma_non_troppo/flute.ly
PIANO = 01_allegro_ma_non_troppo/piano.ly
TEXT = 01_allegro_ma_non_troppo/text.ly

bassoon.pdf: bassoon.ly $(BASSOON) $(TEXT)
	lilypond bassoon.ly

cello.pdf: cello.ly $(CELLO) $(TEXT)
	lilypond cello.ly

flute.pdf: flute.ly $(FLUTE) $(TEXT)
	lilypond flute.ly

piano.pdf: piano.ly $(PIANO) $(TEXT)
	lilypond piano.ly

score.pdf: $(BASSOON) $(CELLO) $(FLUTE) $(PIANO) $(TEXT)
	lilypond score.ly

clean:
	rm -f *.{log,tex,dvi,ps}
