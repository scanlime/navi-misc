\version "2.4.2"

%%%%%%%%%%%%%%%%%%
% Theme A, intro %
%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectOneRight = \relative b' {
  % 1
  r1 r1
  | b8\f d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis, \times 2/3 { fis'16 fis fis } 
  
  % 7
  | b,8\ff d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 c e g fis e fis4
  | <fis, fis,>8 <e e,> <d' d,> <b b,> 
      \times 2/3 { <e e,> <cis cis,> <b b,> } <ais ais,>4~
  
  % 11
  | <ais g ais,>1\mf
  | <g' ais,>
  | <e, ais g'>\arpeggio\f~
  | <e ais g'>
}

mvmtOnePianoSectOneLeft = \relative b {
  % 1
  r1 r1
  | b8 d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis, r

  % 7
  \clef bass
  | <b b,> <d d,> <fis fis,> <b b,> <ais ais,> <gis gis,> <fis fis,>4
  | <g g,>8 <fis fis,> <e e,> <fis fis,> <d d,> <cis cis,> <fis, fis,>4
  | <g g,>8 <c c,> <e e,> <g g,> <fis fis,> <e e,> <fis fis,>4
  | fis,,8 e d' b e g cis4~
  
  % 11
  | <cis e>1
  \clef treble
  | <e cis'>
  \clef bass
  | <e,, cis' b'>\arpeggio~
  | <e cis' b'>
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Theme B, initial development %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectTwoBase = \relative b, {
  % 23
  | b16 d cis d e d cis d b d ees d cis d e-> d
  | f-> d cis d b d fis-> d g-> ees d ees gis-> e dis e
  | g-> d cis d b d fis-> d gis-> e dis e b e d e
  | gis-> e dis e fis d cis d b'-> fis e fis dis fis b-> fis
}

mvmtOnePianoSectTwoRight = \relative b {
  \clef bass
  << \mvmtOnePianoSectTwoBase { s1\f } >>
  
  % 27
  | a16( fis cis fis 
      \clef treble
      cis' a fis' cis) 
      g'( e b e b' dis, fis a)
  | gis( e cis' a) dis( b fis dis' e a, gis eis') fis( a, ais g')
  | fis\<( dis a fis) g( e cis g) dis''( a fis dis)
      \clef bass
      cis( g e b)\!
  | fis\ff( g a g fis a g fis) e cis dis e a, d cis c

  % 31
  | r\fp a' fis g cis fis, a g r a fis g cis fis, a g
  | r a fis g d' g, b a r a fis g d' gis, b a
  | r a fis gis d' gis, b ais r ais fis g e' g, b a
  | r b fis g fis' gis, b cis g' d cis g fis e d cis
  
  % 35
  | b fis' b d fis\< ais b cis d fis,( a b
      \clef treble
      d fis g a)\!
}

mvmtOnePianoSectTwoLeft = \relative b, {
  \transpose b b, \mvmtOnePianoSectTwoBase
  
  % 27
  | a( fis cis fis cis' a fis' cis) 
      g'( e b e b' dis, fis a)
  | gis( e cis' a) dis( b fis dis' e a, gis eis') fis( a, ais g')
  | fis( dis a fis) g( e cis g) dis''( a fis dis) cis( g e b)
  #(set-octavation -1)
  | fis( g a g fis a g fis) e cis dis e a, d cis c
  
  % 31
  | << 
    { b2 b b b b b }
    { b'2 b b b b b }
  >>
  | << b,4 b'4 >>
  #(set-octavation 0)
    fis' g fis,

  % 35
  | b fis'8 d' fis g b cis
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Accompaniment to theme C %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectThreeRight = \relative b' {
  % 36
  | b16\mp a fis d cis'8. b16 g8. cis16 fis8. e16
  | ais,8 g' fis e d8. fis16 b8. a16
  | << 
       { gis2 g2 } \\
       { gis8. b,16 cis8. d16 e8. g,16 a8. b16 }
    >>
  | <<
       { fis'2 eis2 } \\
       { c8. fis,16 g8. a16 b8. cis16 d4 }
    >>
  
  % 40
  | e8. cis16 e8. g16 b8. cis16 fis8. e16
  | g16 cis, fis, cis ais e
      \clef bass
      ais, e d\< fis b d fis, a c e\!
  | d\mf b g d cis fis ais cis d b fis d e a cis e
  | fis\< d a fis e g c e fis dis a fis gis b d gis\!
  
  % 44
  \clef treble
  | fis8\fp e gis4. fis8 d4~
  | d8 f \stemUp cis'4. b8 a g-.\sf \stemNeutral  % XXX sf conflicts
  | fis8 <e b f>-.\sf gis4 r8 gis16 a d8 <e bes>
  
  % 47
  | << { d8\< c e d f e~\! e4 } \\
       { r16 bes a g a f bes c des bes e,8~ e4 } >>
  | << { f'8 e g\> f a g~ g f\! } \\
       { d,16 a' cis d e cis a d c a b g a bes~ bes d } >>
  | g8\mp e'16 f e f d e cis d e8 a16-. e-. a,-. e-.
  | f8 
      \clef bass
      g,,16 a g a f g e f g e c'-. g-. c,-. g-.
  % 51
  | a8\mp
      \clef treble
      a'\glissando a'4
      << { e'8 f~ f g } \\
         { c,16 bes a bes a g f g } >>
  | e8 a,\glissando cis'4
      << { e'16 d-. cis-. d-. cis-. b-. a-. b-. } \\
         { e,8 f~ f g } >>
  | cis8\< e,16 a <cis cis,>8 a16 cis <e e,>8 fis,16 bis <dis dis,>8 cis16 dis\!
  
  % 54
  | <eis eis,>8\f gis,16[ a fis gis] fis[ gis e fis] eis'[ fis dis eis d e]
  | <cis eis, cis>8 a,16 b gis a gis a fis g fis' gis e fis d e
  | <cis gis>8\> r8 r4\! r2

  % 57
  | r1 r1 r1
}

mvmtOnePianoSectThreeLeft = \relative b, {
  % 36 
  | d4 r16 g b r r a cis r r e a r
  | r cis, e r r a, cis r r b d r r a fis' r
  | r d f r r b, d r r cis e r r a, cis r
  | r dis fis r r bis, dis r r b d r r gis, b d,

  % 40
  | cis e g a, r e'' g a, r cis e g, r cis e fis,
  | r1
  | r4 fis, g a 
  | d c b e

  % 44
  | <<
      { r16 a, e' a cis 
          \change Staff = pianoRight \stemDown dis e8 d cis b4 \stemNeutral } \\
      { \change Staff = pianoLeft s2 r16 e,, b' e gis ais b cis }
    >>
  | <<
      { b8 \change Staff = pianoRight \stemDown d g eis fis4 cis8 b8-. \stemNeutral } \\
      { r16 eis,, b' eis ais b cis d r fis,, cis' fis a fis <e a,>8-. }
    >>
  | r16 d <a e'>8-. 
      \clef treble
      b'16 cis d e c a c f b a d, bes

  % 47
  \clef bass
  | r4 f8 fis g gis e,16 c' \change Staff = pianoRight a' e'
  \change Staff = pianoLeft
  | << { r8 e, cis f d a' f c' } \\
       { r4 cis,, d f8 d'8 } >>
  | bes 
      \clef treble
      g'''16 a g a f g e f g cis, a'-. e-. a,-. e-.
  | bes8
      \clef bass
      << { bes,16 c bes c a bes g a bes8 } \\
         { d,8~ d cis~ cis c } >>
      c'16-. g-. c,-. g-.

  % 51
  | a16( e' a e') a e a e eis fis g gis a ais b bis
  | cis16 e,,( a e') a e a e 
        << { cis' d-. dis-. e-. eis-. fis-. g-. gis-. }
           { s16 c,-. b-. bes-. a-. aes-. g-. b-. } >>
  | <a' cis,>8 gis16 e <cis cis,>8 e16 cis <a a,>8 dis16 bis <gis gis,>8 gis16 eis

  % 54
  | <cis cis,>8 
      \clef treble
      <eis' cis'>16[ fis <a d,> cis] <dis e,>[ bis <cis e,> e] bis'[ a <bis dis,> e, <a cis,> d,]
  | <cis, gis'>8
      \clef bass
      <d fis,>16 b <cis eis,> a <e' gis,> a, <d fis,> gis, <b d,> b, <cis g'> d dis <e gis>
  | <cis eis> gis' eis cis fis, d' a e' b' gis b gis a, e' b' gis
  % 57
  | a, fis' b gis b gis a, fis' b gis a, b cis gis b a 
  | e d fis gis e d fis cis b d a gis b4 
  | cis1
}

mvmtOnePianoSectFourRight = \relative cis'' {
  | r2 r4. cis8

  % 69
  \time 6/8
  | cis4. cis4 b8
  | << { cis4.~ cis4 b8 } \\
       { r4 b8~ b4 a8 } >>
  | a4. a4 b8
  | << { a2. } \\
       { f4. c4. } >>
  | b'4. b4 a8
  | b4. a4 b8
  | a4. cis16 d e8-. bis-.

  % 76
  | cis4. cis4~ cis16 b
  | << { cis4.~ cis4 d8 } \\
       { e,4. <d g> } >>
  | << { cis'4. b4~ b16 b } \\
       { <cis, fis>4. <dis fis> } >>
  | << { bis'4.\< bes\! } \\
       { dis,4. f } >>

  % 80
  | << { a4. } \\
       { e4. } >>
    #(set-octavation 1)
    b'''4.
  | g e
  #(set-octavation 0)
  | <bes, ees,> des'
  | aes ees
}

mvmtOnePianoSectFourLeft = \relative a {
  | r1

  % 69
  \time 6/8
  | a8 e' a gis, e' gis
  | g, e' g fis, e' fis
  | f, d' f e, d' fis
  | ees, c' g'~ g f e
  | dis fis b, d f b,
  | << { cis e a, c ees gis, } \\
       { a4. ees } >>
  | << { b'4. } \\
       { d,4. } >>
      \clef treble
      <d' fis a b>8 <d f a b>-. <d e gis>-.

  % 76
  \clef bass
  | a8 e' a eis, b' gis'
  | fis, cis' a' e, b' g'
  | dis, a' fis' a a, dis,
  | a fis' dis' aes, f' d'

  % 80
  | g,, e' cis'
      \clef treble
      \appoggiatura b'8 b'4.
  | \appoggiatura g,8 g'4. \appoggiatura e,8 e'4.
  \clef bass
  | aes,,,8 ees' c'
      \clef treble
      \appoggiatura des8 des'4.
  | \appoggiatura aes,8 aes'4. \clef bass \appoggiatura ees,8 ees'4.
}

mvmtOnePianoRight = {
  \clef treble
  \key b \minor
  \time 4/4

  \mvmtOnePianoSectOneRight
  R1*8
  \mvmtOnePianoSectTwoRight
  \mvmtOnePianoSectThreeRight
  R1*8
  \mvmtOnePianoSectFourRight
}

mvmtOnePianoLeft = {
  \clef treble
  \key b \minor
  \time 4/4
  
  \mvmtOnePianoSectOneLeft
  R1*8
  \mvmtOnePianoSectTwoLeft
  \mvmtOnePianoSectThreeLeft
  R1*8
  \mvmtOnePianoSectFourLeft
}
