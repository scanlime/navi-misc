\include "01_allegro_ma_non_troppo/bassoon.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
		instrument = "BASSOON"
	}

	\score {
		<<
			\context Staff = mvmtOneBassoon <<
				\set Staff.midiInstrument = "bassoon"
				\set Score.skipBars = ##t
                #(set-accidental-style 'modern-voice)
                \context Tempo \mvmtOneTempo
				\mvmtOneBassoon
			>>
		>>
	}
}
