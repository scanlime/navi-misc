\include "text.ly"

serenadeFinaleCello = \notes \relative c {
  \key d \minor
  \time 4/4
  \clef bass
  \once \override TextScript   #'extra-offset = #'(-6.0 . 2.0)
  r8
  ^\serenadeTempo
  aes8
  ^\markup {pizz.}
  a bes cis d ees4\tenuto
}
