\include "prologue-cello.ly"
\include "prologue-piano.ly"

#(set-global-staff-size 14)

\header {
  composer = "CLAUDE DEBUSSY"
  title = "SONATE"
  subtitle = "pour Violoncelle et Piano"
}

\score {
  \context Score <<
    \context Staff=prologueCello <<
      \prologueCello
    >>
    \context PianoStaff <<
      \context Staff=prologuePianoTreble << \prologuePianoTreble >>
%      \new Dynamics                      << \prologueMiddleDynamics >>
      \context Staff=prologuePianoBass   << \prologuePianoBass >>
    >>
  >>
  \paper {
  }
}
