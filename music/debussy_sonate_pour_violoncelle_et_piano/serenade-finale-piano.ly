\include "text.ly"

serenadeFinalePianoTreble = \notes \relative c {
  \key d \minor
  \time 4/4

  \override TextScript   #'extra-offset = #'(-6.0 . 0)
  r1
  ^\serenadeTempo
}

serenadeFinalePianoBass = \notes \relative c, {
  \key d \minor
  \time 4/4
  \clef bass

  r2 r4 r8 <ges ges'>8
}
