\include "text.ly"

finaleCello = \notes \relative c {
  \key d \minor
  \time 2/4
  \clef bass
  \once \override TextScript #'extra-offset = #'(-6.0 . 2.0)
  r4
  ^\finaleTempo
  d8_\staccato_\tenuto
  ^\markup {pizz.}
  r8
  c8_\staccato_\tenuto r8 bes8_\staccato_\tenuto r8
}
