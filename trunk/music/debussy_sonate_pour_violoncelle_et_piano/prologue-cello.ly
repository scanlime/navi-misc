\include "text.ly"

prologueCello = \notes \relative c {
  \key d \minor
  \time 4/4
  \clef bass

  r1
  ^\prologueTempo
  r1
  r1
  r8 d,32 (g d' g d'4\tenuto) ~
  \clef tenor
  d8 (e f e16 d)
  \set subdivideBeams = ##t
  e32 ([g f e d8] ~ d32 [e d c a c a g])
  \clef bass
  e ([g f e d8] ~ d32 [g, d' f a d, g b])
  \clef tenor
  e32
  ^\raiseCedez
  (g, d' f a d, g bes) ~ bes4 ~ bes16 \(ges bes, (c)
  \clef bass
  ees, aes, ees' c\)
  \clef tenor
  bes''2\tenuto ~bes16 (ges bes, c ees f ges aes) \breathe
  r8
  ^\pocoAnimando
  a8 (f e) r8 a16 (f e4) ~
  e8 (d16 c a4 ~ a8 f e d)
  \clef bass
  r8 bes (c d e d16 c bes4 ~
  bes8 a g4) ~ g8 r8 r4
  \clef tenor
  r8 a''8 (f e) r8 (a16 f e4) ~
  e8 (d16 c aes4 ~ aes8 bes c d)
  \clef bass
  r2 r8 a16 (f e4 ~
  e8
  ^\raiseCedez
  d16 c d8 f16 g d4) ~ d8 r8
  \clef tenor
  \set Score.markFormatter = #(lambda (mark  context) (make-bold-markup (make-box-markup (number->string mark))))
  \mark \default
  b'16\tenuto
  ^\auMouvt
  (c\tenuto d\tenuto e\tenuto)
  b\tenuto (cis\tenuto d\tenuto e\tenuto)
  b\tenuto (c\tenuto d\tenuto e\tenuto)
  b\tenuto (c\tenuto d\tenuto e\tenuto) ~
}
