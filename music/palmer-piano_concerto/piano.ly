\include "01_allegro_ma_non_troppo/piano.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}

	\score {
		\context PianoStaff <<
			\set Score.skipBars = ##t
			\set PianoStaff.connectArpeggios = ##t
            #(set-accidental-style 'piano)
			\context Staff = "pianoRight" {
                \set Staff.midiInstrument = "piano"
				\mvmtOnePianoRight
			}
			\context Staff = "pianoLeft" {
                \set Staff.midiInstrument = "piano"
				\mvmtOnePianoLeft
			}
		>>
	}
}
