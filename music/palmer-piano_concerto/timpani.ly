\include "01_allegro_ma_non_troppo/timpani.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
		instrument = "TIMPANI"
	}

	\score {
		<<
			\context Staff = mvmtOneTimpani <<
				\set Staff.midiInstrument = "timpani"
				\set Score.skipBars = ##t
                		#(set-accidental-style 'modern-voice)
                \context Tempo \mvmtOneTempo
				\mvmtOneTimpani
			>>
		>>
	}
}
