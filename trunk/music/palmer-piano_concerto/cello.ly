\include "01_allegro_ma_non_troppo/cello.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
		instrument = "VIOLONCELLO"
	}

	\score {
		<<
			\context Staff = mvmtOneCello <<
				\set Staff.midiInstrument = "cello"
				\set Score.skipBars = ##t
                #(set-accidental-style 'modern-voice)
				\keepWithTag #'part \mvmtOneCello
			>>
		>>
	}
	\paper {
		#(set-paper-size "letter")
	}
}
