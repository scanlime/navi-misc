all: bassoon.pdf cello.pdf flute.pdf piano.pdf score.pdf timpani.pdf

BASSOON = 01_allegro_ma_non_troppo/bassoon.ly
CELLO = 01_allegro_ma_non_troppo/cello.ly
FLUTE = 01_allegro_ma_non_troppo/flute.ly
PIANO = 01_allegro_ma_non_troppo/piano.ly
TEXT = 01_allegro_ma_non_troppo/text.ly
TIMPANI = 01_allegro_ma_non_troppo/timpani.ly

bassoon.pdf: bassoon.ly $(BASSOON) $(TEXT)
	lilypond bassoon.ly

cello.pdf: cello.ly $(CELLO) $(TEXT)
	lilypond cello.ly

flute.pdf: flute.ly $(FLUTE) $(TEXT)
	lilypond flute.ly

piano.pdf: piano.ly $(PIANO) $(TEXT)
	lilypond piano.ly

score.pdf: score.ly $(BASSOON) $(CELLO) $(FLUTE) $(PIANO) $(TEXT) $(TIMPANI)
	lilypond score.ly

timpani.pdf: timpani.ly $(TIMPANI) $(TEXT)
	lilypond timpani.ly

clean:
	rm -f *.{log,tex,dvi,ps}
