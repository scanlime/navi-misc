\include "01_allegro_ma_non_troppo/piano.ly"
\include "01_allegro_ma_non_troppo/tempo.ly"
\include "context.ly"

% from lilypond-documentation
% pasted here because I can't figure out how to factor it out

\version "2.3.22"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}
    \score {
     \context PianoStaff <<
       \set Score.skipBars = ##t
       \context Tempo=tempo \mvmtOneTempo
       \context Staff=pianoRight \mvmtOnePianoRight
       \context Dynamics=pianoDynamics \mvmtOnePianoDynamics
       \context Staff=pianoLeft \mvmtOnePianoLeft
       \context Dynamics=pedal \mvmtOnePianoPedal
     >>
   }
}
