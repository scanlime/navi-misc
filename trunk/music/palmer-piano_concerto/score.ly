\include "01_allegro_ma_non_troppo/bassoon.ly"
\include "01_allegro_ma_non_troppo/cello.ly"
\include "01_allegro_ma_non_troppo/flute.ly"
\include "01_allegro_ma_non_troppo/piano.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}
	\score {
		<<
			\context StaffGroup = "winds" <<
				\context Staff = "flute" {
					\set Staff.instrument = "Flute"
					\set Staff.instr = "Fl."
					\mvmtOneFlute
				}
				\context Staff = "bassoon" {
					\set Staff.instrument = "Bassoon"
					\set Staff.instr = "Bsn."
					\mvmtOneBassoon
				}
			>>
			\context PianoStaff <<
				\set PianoStaff.connectArpeggios = ##t
				\set PianoStaff.instrument = "Piano"
				\set PianoStaff.instr = "Pno."
				#(set-accidental-style 'piano)
				\context Staff = "pianoRight" {
					\mvmtOnePianoRight
				}
				\context Staff = "pianoLeft" {
					\mvmtOnePianoLeft
				}
			>>
			\context StaffGroup = "strings" <<
				\context Staff = "cello" {
					\set Staff.instrument = "Cello"
					\set Staff.instr = "Vc."
					\mvmtOneCello
				}
			>>
		>>
		\layout {
			\context { \RemoveEmptyStaffContext }
		}
	}
}
