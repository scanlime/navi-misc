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
			\set Staff.midiInstrument = "piano"
			\context Staff = "pianoRight" {
				\mvmtOnePianoRight
			}
			\context Staff = "pianoLeft" {
				\mvmtOnePianoLeft
			}
		>>
	}
}
