\include "01_allegro_ma_non_troppo/bass.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
		instrument = "CONTRABASS"
	}

	\score {
		<<
			\context Staff = mvmtOneBass <<
				\set Staff.midiInstrument = "contrabass"
				\set Score.skipBars = ##t
                		#(set-accidental-style 'modern-voice)
                \context Tempo \mvmtOneTempo
				\mvmtOneBass
			>>
		>>
	}
}
