all: bass.pdf bassoon.pdf cello.pdf flute.pdf piano.pdf score.pdf timpani.pdf

BASS = 01_allegro_ma_non_troppo/bass.ly
BASSOON = 01_allegro_ma_non_troppo/bassoon.ly
CELLO = 01_allegro_ma_non_troppo/cello.ly
CONTEXT = context.ly
FLUTE = 01_allegro_ma_non_troppo/flute.ly
PIANO = 01_allegro_ma_non_troppo/piano.ly
TEMPO = 01_allegro_ma_non_troppo/tempo.ly
TEXT = 01_allegro_ma_non_troppo/text.ly
TIMPANI = 01_allegro_ma_non_troppo/timpani.ly

bass.pdf: bass.ly $(BASS) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond bass.ly

bassoon.pdf: bassoon.ly $(BASSOON) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond bassoon.ly

cello.pdf: cello.ly $(CELLO) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond cello.ly

flute.pdf: flute.ly $(FLUTE) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond flute.ly

piano.pdf: piano.ly $(PIANO) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond piano.ly

score.pdf: score.ly $(BASS) $(BASSOON) $(CELLO) $(FLUTE) $(PIANO) $(TEXT) $(TIMPANI) $(TEMPO) $(CONTEXT)
	lilypond score.ly

timpani.pdf: timpani.ly $(TIMPANI) $(TEXT) $(TEMPO) $(CONTEXT)
	lilypond timpani.ly

clean:
	rm -f *.{log,tex,dvi,ps,pdf}
