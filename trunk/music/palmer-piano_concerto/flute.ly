\include "01_allegro_ma_non_troppo/flute.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
		instrument = "FLUTE"
	}

	\score {
		<<
			\context Staff = mvmtOneBassoon <<
				\set Staff.midiInstrument = "flute"
				\set Score.skipBars = ##t
                #(set-accidental-style 'modern-voice)
                \context Tempo \mvmtOneTempo
				\mvmtOneFlute
			>>
		>>
	}
}
