\include "text.ly"

prologueCello = \notes \relative c {
  \key d \minor
  \time 4/4
  \clef bass

  \once \override TextScript #'extra-offset = #'(0 . 2.0)
  r1
  ^\prologueTempo
  r1
  r1
  r8
  \phrasingSlurDown
  \override PhrasingSlur #'attachment-offset = #'((0.5 . 0) . (2 . 0))
  d,32\f \(g d' g\) d'4\tenuto (
  \clef tenor
  d8)
  \override PhrasingSlur #'attachment-offset = #'((-2.5 . 0) . (0 . 0))
  \phrasingSlurUp
  e \(f e16 d\)
  \revert Slur #'attachment-offset
  \revert PhrasingSlur #'attachment-offset
  \set subdivideBeams = ##t
  e32 ([g f e d8] ~ d32 [e d c a c a g])
  \clef bass
  \set decrescendoText = \markup { \bold \italic "dim. " }
  \set decrescendoSpanner = #'dashed-line
  e ([g f e d8] ~
  \>
  d32
  [g, d' f a d, g b])
  \clef tenor
  \once \override TextScript #'extra-offset = #'(0 . 3.0)
  e32
  ^\cedez
  (g, d' f a d, g bes) ~ bes4 ~
  \!\p
  \setHairpinCresc
  bes16\>
  \(ges bes, (c)
  \clef bass
  ees, aes, ees' c\)\!
  \clef tenor
  bes''2\tenuto ~bes16\> (ges bes, c ees f ges aes)\! \breathe
  \once \override TextScript #'extra-offset = #'(0 . 3.0)
  r8
  ^\pocoAnimando
  a8
  _\markup {\bold \italic "dolce sostenuto"}
  (f e) r8 a16 (f e4) ~
  e8 (d16 c a4 ~ a8\> f e d\!)
  \clef bass
  r8 bes
  _\markup {\bold \italic "più dolce"}
  (c d e d16 c bes4 ~
  bes8\> a g4)\! ~ g8 r8 r4
  \clef tenor
  r8 a''8
  _\markup {\bold \italic "più" \dynamic p}
  (f e) r8 (a16 f e4) ~
  e8 (d16 c aes4 ~ aes8\< bes c d\!)
  \clef bass
  r2 r8 a16\pp (f e4 ~
  \once \override TextScript #'extra-offset = #'(0 . 3.0)
  e8
  ^\cedez
  \>
  d16
  \startTextSpan
  c d8 f16 g d4\!) ~ d8
  \stopTextSpan
  r8
  \clef tenor
  \set Score.markFormatter = #(lambda (mark  context) (make-bold-markup (make-box-markup (number->string mark))))
  \mark \default
  \once \override TextScript #'extra-offset = #'(0 . 2.0)
  b'16\tenuto
  ^\auMouvt
  \p\<
  (c\tenuto d\tenuto e\tenuto)
  b\tenuto\p\< (cis\tenuto d\tenuto e\tenuto)
  b\tenuto\p\< (c\tenuto d\tenuto e\tenuto)
  b\tenuto\p\< (c\tenuto d\tenuto e\tenuto\!) ~
}
