\include "01_allegro_ma_non_troppo/cello.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}

	\score {
		<<
			\context Staff = mvmtOneCello <<
				\set Staff.midiInstrument = "cello"
				\mvmtOneCello
			>>
		>>
	}
}
