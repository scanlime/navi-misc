\include "01_allegro_ma_non_troppo/flute.ly"

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
				\mvmtOneFlute
			>>
		>>
	}
}
