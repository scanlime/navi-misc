\version "2.4.2"

%%%%%%%%%%%%%%%%%%
% Theme A, intro %
%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectOneRight = \relative b' {
  r1 r1
  | b8\f d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis, \times 2/3 { fis'16 fis fis } 
  
  | b,8\ff d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 c e g fis e fis4
  | <fis, fis,>8 <e e,> <d' d,> <b b,> 
      \times 2/3 { <e e,> <cis cis,> <b b,> } <ais ais,>4~
  
  | <ais g ais,>1\mf
  | <g' ais,>
  | <e, ais g'>\arpeggio\f~
  | <e ais g'>
}

mvmtOnePianoSectOneLeft = \relative b {
  r1 r1
  | b8 d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis, r

  \clef bass
  | <b b,> <d d,> <fis fis,> <b b,> <ais ais,> <gis gis,> <fis fis,>4
  | <g g,>8 <fis fis,> <e e,> <fis fis,> <d d,> <cis cis,> <fis, fis,>4
  | <g g,>8 <c c,> <e e,> <g g,> <fis fis,> <e e,> <fis fis,>4
  | fis,,8 e d' b e g cis4~
  
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
  | b16 d cis d e d cis d b d ees d cis d e-> d
  | f-> d cis d b d fis-> d g-> ees d ees gis-> e dis e
  | g-> d cis d b d fis-> d gis-> e dis e b e d e
  | gis-> e dis e fis d cis d b'-> fis e fis dis fis b-> fis
}

mvmtOnePianoSectTwoRight = \relative b {
  \clef bass
  << \mvmtOnePianoSectTwoBase { s1\f } >>
  | a16( fis cis fis 
      \clef treble
      cis' a fis' cis) 
      g'( e b e b' dis, fis a)
  | gis( e cis' a) dis( b fis dis' e a, gis eis') fis( a, ais g')
  | fis\<( dis a fis) g( e cis g) dis''( a fis dis)
      \clef bass
      cis( g e b)\!
  | fis\ff( g a g fis a g fis) e cis dis e a, d cis c

  | r\fp a' fis g cis fis, a g r a fis g cis fis, a g
  | r a fis g d' g, b a r a fis g d' gis, b a
  | r a fis gis d' gis, b ais r ais fis g e' g, b a
  | r b fis g fis' gis, b cis g' d cis g fis e d cis
  | b fis' b d fis\< ais b cis d fis,( a b
      \clef treble
      d fis g a)\!
}

mvmtOnePianoSectTwoLeft = \relative b, {
  \transpose b b, \mvmtOnePianoSectTwoBase
  | a( fis cis fis cis' a fis' cis) 
      g'( e b e b' dis, fis a)
  | gis( e cis' a) dis( b fis dis' e a, gis eis') fis( a, ais g')
  | fis( dis a fis) g( e cis g) dis''( a fis dis) cis( g e b)
  #(set-octavation -1)
  | fis( g a g fis a g fis) e cis dis e a, d cis c
  
  | << 
    { b2 b b b b b }
    { b'2 b b b b b }
  >>
  | << b,4 b'4 >>
  #(set-octavation 0)
    fis' g fis,
  | b fis'8 d' fis g b cis
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Accompaniment to theme C %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectThreeRight = \relative b' {
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
  | e8. cis16 e8. g16 b8. cis16 fis8. e16
  | g16 cis, fis, cis ais e
      \clef bass
      ais, e d\< fis b d fis, a c e\!
  | d\mf b g d cis fis ais cis d b fis d e a cis e
  | fis\< d a fis e g c e fis dis a fis gis b d gis\!
  \clef treble
  | fis8\fp e gis4. fis8 d4~
  | d8 f \stemUp cis'4. b8 a g-.\sf \stemNeutral  % XXX sf conflicts
}

mvmtOnePianoSectThreeLeft = \relative b, {
  | d4 r16 g b r r a cis r r e a r
  | r cis, e r r a, cis r r b d r r a fis' r
  | r d f r r b, d r r cis e r r a, cis r
  | r dis fis r r bis, dis r r b d r r gis, b d,
  | cis e g a, r e'' g a, r cis e g, r cis e fis,
  | r1
  | r4 fis, g a 
  | d c b e
  | <<
      { r16 a, e' a cis 
          \change Staff = pianoRight \stemDown dis e8 d cis b4 \stemNeutral } \\
      { \change Staff = pianoLeft s2 r16 e,, b' e gis ais b cis }
    >>
  | <<
      { b8 \change Staff = pianoRight \stemDown d g eis fis4 cis8 b8-. \stemNeutral } \\
      { r16 eis,, b' eis ais b cis d r fis,, cis' fis a fis <e a,>8-. }
    >>
}

mvmtOnePianoRight = {
  \clef treble
  \key b \minor
  \time 4/4

  \mvmtOnePianoSectOneRight
  R1*8
  \mvmtOnePianoSectTwoRight
  \mvmtOnePianoSectThreeRight
}

mvmtOnePianoLeft = {
  \clef treble
  \key b \minor
  \time 4/4
  
  \mvmtOnePianoSectOneLeft
  R1*8
  \mvmtOnePianoSectTwoLeft
  \mvmtOnePianoSectThreeLeft
}
