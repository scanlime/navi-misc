\include "text.ly"

u = {
  \change Staff = prologuePianoTreble
}

d = {
  \change Staff = prologuePianoBass
  \stemUp
}

prologuePianoTreble = \new Voice \notes\relative c' {
  \key d \minor
  << {
    <f a>1\marcato
    ^\raisePrologueTempo
    <f a>2. (g8 e8)
    <f a>2.\tenuto\marcato a8 b16 c
    \times 2/3 {c16\accent (b a g8.)} r4
  } \\ {
    r8 r16 \times 2/3 {c,32 [d e]} d4. f8 e d16 c ~
    c8 bis16 a bis2 <g c>4
    r8 r16 \times 2/3 {c32 [d e]} d4.  f8 ~ f4
    <d g>8 (\times 2/3 {c16_\accent b a} g8) r8
  } >>
  r2
  <g a c>8\arpeggio <g b d>\arpeggio r4 r2
  << {
    r4
    ^\cedez
    <ees' ges>2.\tenuto
    \override Slur #'attachment-offset = #'((0 . 0) . (0 . -4))
    \slurDown
    f4
    <ees ges>2.\tenuto
    \slurUp
    <d f>2.
    ^\pocoAnimando
    (<d gis>4
    <d f>1)
  } \\ {
    r2 ees,4 ~ ees8 r8
    r2 ees4 ~ ees8 r8
    d'4 (cis c b)
    d (cis c b)
  } >>
  <a d f>4 (<g c e> <f bes d> <g c e>
  <a d f> <bes ees g> <c f a> <d g bes>)
  << {
    \stemDown a'4 (aes) a (aes)
    %{
    << {
      \stemUp c2 (d4 e4)
    } \\ {
      \tieUp
      \stemDown a,4 \stemUp aes ~ aes ~ aes
    } >>
    %}
  } \\ {
    \stemUp <f c'>2\tenuto <f c'>\tenuto
%    f4 (\stemDown e2 \stemUp d4)
  } >>
  << {
    \stemUp c'2 (d4 e)
  } \\ {
    << {
      \stemDown a,4
      \stemUp aes ~ aes ~ aes
    } \\ {
      f4 (e2 \stemUp d4)
    } >>
  } >>
  r1
  r1
  r1
  ^\auMouvt
}

prologuePianoBass = \new Voice \notes\relative c {
  \key d \minor
  \clef bass
  <d a'>1\marcato
  <g, d'>2. (<a e'>4)
  << {
    <a f'>1
  } \\ {
    d,2 c4 a
  } >>
  <g g'>4 ~ <g g'>8 r8 r2
  <c g' e'>8\arpeggio <g d' g d'>\arpeggio r4 r2
  r2 r4 <aes aes'>8 r8
  r2 r4 <aes aes'>8 r8
  r1
  r1
  r1
  r1
  f'4 (bes,) f' (bes,)
  f' (bes,) f' (bes,)
  << {
  } \\ {
  } >>
  r1
  r1
  r1
}

prologueMiddleDynamics = \notes {
%  \override Dynamics.TextScript  #'padding = #-1 %tweak
  s1\f
}
