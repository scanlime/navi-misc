\include "prologue-cello.ly"
\include "prologue-piano.ly"
\include "serenade-cello.ly"
\include "serenade-piano.ly"

#(set-global-staff-size 16)

\header {
  composer = "CLAUDE DEBUSSY"
  title = "SONATE"
  instrument = "pour Violoncelle et Piano"
}

\score {
  \context Score <<
    \context Staff=prologueCello <<
      \set Staff.instrument = \markup {
        \small VIOLONCELLE
      }
      \override Staff.StaffSymbol #'staff-space = #0.85
      \tiny
      \prologueCello
    >>
    \context PianoStaff <<
      \set PianoStaff.instrument = \markup {
        \large PIANO
      }
      \override PianoStaff.VerticalAlignment #'forced-distance = #13
      \context Staff=prologuePianoTreble << \prologuePianoTreble >>
%      \new Dynamics                      << \prologueMiddleDynamics >>
      \context Staff=prologuePianoBass   << \prologuePianoBass >>
    >>
  >>
  \header {
    piece = "Prologue"
  }
}

\score {
  \context Score <<
    \context Staff=serenadeCello <<
      \serenadeCello
    >>
    \context PianoStaff <<
    \context Staff=serenadePianoTreble << \serenadePianoTreble >>
    \context Staff=serenadePianoBass   << \serenadePianoBass >>
    >>
  >>
  \header {
    piece = "Sérénade"
  }
}
