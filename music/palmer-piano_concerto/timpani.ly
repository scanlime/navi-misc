\include "01_allegro_ma_non_troppo/timpani.ly"

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
				\mvmtOneTimpani
			>>
		>>
	}
}
