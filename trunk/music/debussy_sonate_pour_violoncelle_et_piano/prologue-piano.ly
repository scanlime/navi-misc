\include "text.ly"

u = {
  \change Staff = prologuePianoTreble
  \stemDown
}

d = {
  \change Staff = prologuePianoBass
  \stemUp
}

prologuePianoTreble = \new Voice \notes\relative c' {
  \key d \minor
  << {
    <f a>1\marcato
    ^\prologueTempo
    <f a>2. (g8 e8)
    <f a>2.\tenuto\marcato a8 b16 c
    \once \override Slur #'attachment-offset = #'((0 . 2) . (0 . 2))
    \times 2/3 {c16\accent (b a)} <d, g>8 r4
  } \\ {
    \once \override DynamicText #'extra-offset = #'(0 . -2.0)
    r8\f
    \once \override Rest #'extra-offset = #'(0 . -1.8)
    r16 \times 2/3 {c32 [d e]} d4. f8 e d16 c ~
    c8 bis16\< a bis2 <g c>4\!
    \once \override TextScript #'extra-offset = #'(-1 . 0)
    r8
    _\markup {\bold \italic "più" \dynamic f}
    \once \override Rest #'extra-offset = #'(0 . -1.8)
    r16 \times 2/3 {c32 [d e]} d4.  f8\< ~ f4\!
    \once \override Slur #'attachment = #'(stem . stem)
    \once \override Slur #'attachment-offset = #'((0 . 1) . (0 . 1))
    <d g>8 (\times 2/3 {c16_\accent b a} g8) r8
  } >>
  r2
  <g a c>8\arpeggio\fz <g b d>\arpeggio r4 r2
  << {
    r4
    ^\cedez
    <ees' ges>2.\tenuto\p
    \override Slur #'attachment-offset = #'((0 . 0) . (0 . -4))
    \slurDown
    f4
    <ees ges>2.\tenuto
    _\markup {\bold \italic "più" \dynamic p}
    \slurUp
    <d f>2.
    ^\pocoAnimando
    (<d gis>4
    <d f>1)
  } \\ {
    r2 ees,4 ~ ees8 r8
    r2 ees4 ~ ees8 r8
    \once \override TextScript #'extra-offset = #'(0 . -2.0)
    d'4
    _\markup {\bold \italic "sempre" \dynamic pp}
    (cis c b)
    d (cis c\> b\!)
  } >>
  \once \override TextScript #'extra-offset = #'(0 . -1.4)
  \slurUp
  <a d f>4
  _\markup {\bold \italic "più dolce"}
  (<g c e> <f bes d> <g c e>
  <a d f>\> <bes ees g> <c f a> <d g bes>\!)
  << {
    \stemDown a'4 (aes) a (aes)
  } \\ {
    \stemUp <f c'>2\tenuto
    _\markup {\bold \italic "più" \dynamic p}
    <f c'>\tenuto
  } >>
  << {
    \stemUp c'2 (d4 e)
    <a, f'a>4 (<a f' a>) <a f' a> (<a f' a>)
  } \\ {
    << {
      \stemDown a4
      \stemUp aes ~ aes ~ aes
    } \\ {
      f4 (e2\< \stemUp d4\!)
    } >>
    \stemDown
    cis'2 cis
  } >>
  \clef bass
  <a, c e>4
  ^\cedez
  (<g b d> <f a d>) ~ <f a d>8 r8
  \clef treble
  << {
    <a e'>2
    ^\auMouvt
    <a e'> ~
%    e8 (f g a a) r8 r4
  } \\ {
    bes4\p\< (cis)\! bes\p\< (cis)\!
  } >>
  r1
  r1
  r1
  r1
  \clef bass
  \slurUp
%  \set Voice.beatLength = #(ly:make-moment 1 8)
  \set Voice.subdivideBeams = ##t
  \d des,,32 ([ \u g e' g, \d bes, \u e cis' e, \d des \u g e' g, \d e \u a fis' a,]
  \d des, [\u g e' g, \d e \u a fis' a, \d g \u cis a' cis, \d e, \u a fis' a,]) ~
  \set Voice.subdivideBeams = ##f
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
    <g g'>4 (<fis fis'>) <g g'> (<fis fis'>)
  } \\ {
    cis'2 cis
  } >>
  <a e'>4 (<g d'> <d a' d>) ~ <d, d' a' d>8 r8
  <a''' e'>2 <a e'>
  r1
  r1
  r1
  r1
  \stemDown
  \slurDown
  \once \override Slur #'attachment-offset = #'((0 . 1) . (0 . 1))
  des,8_\staccato (bes_\staccato des_\staccato e_\staccato
  des_\staccato e_\staccato g_\staccato e_\staccato)
}

prologueMiddleDynamics = \notes {
%  \override Dynamics.TextScript  #'padding = #-1 %tweak
  s1\f
}
