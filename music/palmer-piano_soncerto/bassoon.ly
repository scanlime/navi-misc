\include "01_allegro_ma_non_troppo/bassoon.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}

	\score {
		<<
			\context Staff = mvmtOneBassoon <<
				\set Staff.midiInstrument = "bassoon"
				\set Score.skipBars = ##t
				\mvmtOneBassoon
			>>
		>>
	}
}
