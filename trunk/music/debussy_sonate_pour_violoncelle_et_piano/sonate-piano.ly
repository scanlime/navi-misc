\include "prologue-cello.ly"
\include "prologue-piano.ly"

#(set-global-staff-size 16)

\header {
  composer = "CLAUDE DEBUSSY"
  title = "SONATE"
  instrument = "pour Violoncelle et Piano"
}

\score {
  \context Score <<
    \context Staff=prologueCello <<
      \override Staff.StaffSymbol #'staff-space = #0.85
      \tiny
      \prologueCello
    >>
    \context PianoStaff <<
      \context Staff=prologuePianoTreble << \prologuePianoTreble >>
%      \new Dynamics                      << \prologueMiddleDynamics >>
      \context Staff=prologuePianoBass   << \prologuePianoBass >>
    >>
  >>
  \header {
    piece = "Prologue"
  }
  \paper {
  }
}
