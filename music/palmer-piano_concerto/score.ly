\include "01_allegro_ma_non_troppo/bass.ly"
\include "01_allegro_ma_non_troppo/bassoon.ly"
\include "01_allegro_ma_non_troppo/cello.ly"
\include "01_allegro_ma_non_troppo/flute.ly"
\include "01_allegro_ma_non_troppo/piano.ly"
\include "01_allegro_ma_non_troppo/timpani.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}
	\score {
		<<
            \context Tempo \mvmtOneTempo
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
			\context Staff = "timpani" {
				\set Staff.instrument = "Timpani"
				\set Staff.instr = "Timp."
				\mvmtOneTimpani
			}
			\context PianoStaff <<
				\set PianoStaff.connectArpeggios = ##t
				\set PianoStaff.instrument = "Piano"
				\set PianoStaff.instr = "Pno."
				#(set-accidental-style 'piano)
                \context Staff=pianoRight \mvmtOnePianoRight
                \context Dynamics=pianoDynamics \mvmtOnePianoDynamics
                \context Staff=pianoLeft \mvmtOnePianoLeft
                \context Dynamics=pianoPedal \mvmtOnePianoPedal
			>>
			\context StaffGroup = "strings" <<
				\context Staff = "cello" {
					\set Staff.instrument = "Cello"
					\set Staff.instr = "Vc."
					\mvmtOneCello
				}
				\context Staff = "bass" {
					\set Staff.instrument = "Bass"
					\set Staff.instr = "Cb."
					\mvmtOneBass
				}
			>>
		>>
		\layout {
%			\context { \RemoveEmptyStaffContext }
		}
	}
	\paper {
		#(set-paper-size "letter")
	}
}
