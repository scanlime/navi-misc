\include "prologue-cello.ly"

% #(set-global-staff-size 14)

\header {
  composer = "CLAUDE DEBUSSY"
  title = "SONATE"
  subtitle = "pour Violoncelle et Piano"
}

\score {
  \context Staff=prologueCello <<
    \prologueCello
  >>
  \paper {
  }
}
