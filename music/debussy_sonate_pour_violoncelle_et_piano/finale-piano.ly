\include "text.ly"

finalePianoTreble = \notes \relative c'' {
  \key d \minor
  \time 2/4

  \override Slur #'attachment = #'(stem . stem)
  \override Slur #'attachment-offset = #'((0 . 2) . (0 . 2))
  \slurUp
  r4 \times 2/3 {r16 a16 (d, ~} \times 2/3 {d a' d,}
     \times 2/3 {r16 g c, ~}    \times 2/3 {c g' c,}
     \times 2/3 {r16 f bes, ~}  \times 2/3 {bes f' bes,)}
}

finalePianoBass = \notes \relative c, {
  \key d \minor
  \time 2/4
  \clef bass

  r4
  << {
    e''8\staccato (f\staccato)
    d8\staccato (e\staccato)
    c8\staccato (d\staccato)
  } \\ {
    <d, a'>8\arpeggio_\staccato r8
    <c g'>8\arpeggio_\staccato r8
    <bes f'>8\arpeggio_\staccato r8
  } >>
}
