\include "prologue-cello.ly"

\header {
  composer = "CLAUDE DEBUSSY"
  title = "SONATE"
  instrument = "pour Violoncelle et Piano"
}

\score {
  \context Staff=prologueCello <<
    \prologueCello
  >>
  \header {
    piece = "Prologue"
  }
  \paper {
  }
}
