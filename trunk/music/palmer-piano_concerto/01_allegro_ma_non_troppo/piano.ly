\version "2.4.2"
\include "01_allegro_ma_non_troppo/text.ly"

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Section 4, introduction of the pretty "main theme" %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
  | gis,,8 b dis16 fis e8 <cis e a>\arpeggio e'
  | <b fis d> d fis16 a g8 <ais, e' fis cis'>\arpeggio <g' g'>
  | <fis fis'> r8 \grace {
        \change Staff = pianoLeft
        b,,,8[ fis' d' 
        \change Staff = pianoRight
        ais' cis e] } 
    <d b' fis'>8-. <g cis e>8-. r4  % XXX: divide up between hands?
  | <b b'>8 r8 \grace {
        \change Staff = pianoLeft
        d,,,8[ a' e' fis
        \change Staff = pianoRight
        cis' cis'] } <fis, d' b'>-. <fis dis' gis>-. r4
  | r2.
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
  | r4. r8 <a,, g'>4\arpeggio
  | <d d,>4. fis,
  | s2.
  | s2.
  | r2.
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Section 5, begin development with first theme %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectFiveRight = \relative b' {
  % 93
  | b8 d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis,4
  
  % 97
  \clef bass
  | b,,16 d cis d e d cis d b d ees d cis d e d
  | e b ais b d fis eis fis gis cis, b' eis, cis' gis e' b
  \clef treble
  | g' b ais b f' b, ais b a' b, g' b, f' c ees c
  | e b e, b' e, ais e' ais, d a d, a' c a <f b>8-.->
  | <ees c'>8-.-> r8 r4 r2
}

mvmtOnePianoSectFiveLeft = \relative b {
  % 93
  \clef treble
  | b8 d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 b d g fis e fis4
  | g,8 e d' b gis' eis fis,4
  
  % 97
  \clef bass
  | b,,16 d cis d e d cis d b d ees d cis d e d
  | e b ais b d fis eis fis gis cis, b' eis, cis' gis e' b
  | g' b ais b f' b, ais b a' b, g' b, f' c ees c
  | e b e, b' e, ais e' ais, d a d, a' c a <d, g>8-.->
  | <c g'>8-.-> r8 r4 r2
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Section 6, introduce developmental theme %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectSixRight = \relative b'' {
  % 110
  | r1
  | r2. fis4
  | e2 d
  | cis8 <fis fis,>~ <fis fis,> <fis fis,>~ <fis fis,> <g g,>~ <g g,> <a a,>~
  
  << {
      | <a a,> <g g,>~ <g g,> <fis fis,>~ <fis fis,> <eis eis,>~ <eis eis,> <fis fis,>~
      | <fis fis,> <e e,>~ <e e,> <d d,>~ <d d,> <a' a,>~ <a a,> <g g,>~
      | <g g,> <g g,>~ <g g,> <b b,>~ <b b,> <eis, eis,>~ <eis eis,> <g g,>~
      | <g g,> <fis fis,>~ <fis fis,> <e e,>~ <e e,> <d d,>~ <d d,> <cis cis,>
    } \\ {
      | r2 cis
      | a fis
      | c' d
      | <d b> <cis ais>
    } >>

  % 118
  | r16 b, fis' cis r ais g' c, r a' e b r bis' g d
  | r \times 2/3 { g' d a } e r g, c e r \times 2/3 { fis' cis a } d, r a d fis
  | r g, \times 2/3 { b c e } \times 4/5 { a g dis b g } 
    \times 4/5 { fis ais cis e g} \times 4/5 { d' ais eis cis bis }
  | << { \times 4/5 { fis'' c g fis dis } r g \times 2/3 { fis g a }
         r cis, d e r cis \times 2/3 { d e fis } } \\
       { r4 <a fis cis> <g e b> <fis cis ais> } >>
  
  % 122
  | \times 2/3 { g16 d b } ais b d a' fis d
    << { cis'8. b16 ais8. g16 } \\
       { r16 fis eis r r d cis r } >>
  | fis32 g fis e \times 2/3 { fis16 cis' b } a fis' d a
    << { a'8. g16 fis8. d16 } \\
       { r16 f, e r r ees c r } >>
  
  % 124
  | <eis gis, eis>4 s2.
  #(set-octavation 1)
  | << { 
         | cis''16 gis' cis, gis' cis, gis' cis, gis' gis gis gis gis gis gis gis gis 
         | gis gis gis gis gis gis gis gis gis gis gis gis gis gis a b
       } \\ { 
         | r32 gis,16 gis b b a a gis gis d' d e e d d cis cis32~
         | cis32 dis16 dis dis dis dis eis dis eis dis eis dis eis dis eis dis eis32
       } >>
  | <eis' cis>32 cis b a gis e cis b
    #(set-octavation 0)
    a gis eis cis b a gis eis 
    \change Staff = pianoLeft
    \bar ""
    cis b a gis eis cis b a gis' eis cis b a gis eis d
  \change Staff = pianoRight
  \clef bass
  | cis'16. gis16 eis32 d'16 cis16. gis16 eis32 d'16 cis16. gis16 eis d' cis d cis d32
  | fis,8 fis'32 a cis fis
    \clef treble
    <a cis,>4 
    \clef bass
    fis,,8
    \clef treble
    a'32 cis fis a <cis fis,>4
  | \clef bass
    cis,,8
    \clef treble
    b''32 gis d b
    \clef bass
    <cis eis,>4 d,8 
    \clef treble
    fis''32 d a fis <a d,>4
  | \clef bass
    b,,8 fis'32 b d fis
    \clef treble
    <b d,>8 d,32 fis b d <fis b,>8 b,32 d fis b <d fis,>4
  | << { <cis eis,>4 s <e b gis eis> s } \\
       { <gis,, gis,>4 s <cis cis,> s } >>
  | <fis' d a fis>4( <e cis a e>) r2
  | r2 r4 <a, fis d b>4
  | <d, b gis d> 
    << { <cis a fis>~ <cis a fis> } \\ { d, dis } >> 
    <b' gis eis>
  | <a fis cis a>2 <fis dis b>
}

mvmtOnePianoSectSixLeft = \relative b, {
  % 110
  | b4 cis b d 
  | cis bes a c 
  | aes g b eis,
  | fis a' \appoggiatura { g8[ a] } g4 ees
  
  | \appoggiatura { g8[ a] } b4 cis \appoggiatura { b8[ cis] } b4 d
  | cis bes \appoggiatura { ces8[ bes] } a4 c
  | aes g \appoggiatura { aes8[ ais] } b4 eis,
  | fis <a, a,> \appoggiatura { g8[ a] } <g g,>4 <eis eis,>

  % 118
  | \appoggiatura { g8[ a] } <b b,>4 <cis cis,> <b b,> <d d,>
  | <cis cis,> <bes bes,> <a a,> <c c,>
  | <aes aes,> <g g,> <b b,> <eis, eis,> 
  | <fis fis,> <a' a,> <g g,> <fis fis,>

  % 122
  | <e e,>2 ais16 d cis gis g b a e
  | <d d,>2 ais'16 d cis a d, b' a fis

  % 124
  | << { r8 \times 2/3 { b16 a gis }
         \times 2/3 { cis eis cis } \times 2/3 { bis cis 
           \change Staff = pianoRight gis' }
         \times 2/3 { b a gis } \times 2/3 { cis eis cis }
         \times 2/3 { bis cis gis' } \times 2/3 { b a gis } } \\
       { \change Staff = pianoLeft
         <cis,,, cis,>2 s2 } >>
  \clef treble
  | eis''2 \trill b' \trill
  | b1 \trill
  \clef bass
  | s1
  #(set-octavation -1)
  | cis,,,,16 gis eis d' cis gis eis d' cis gis eis d' cis d cis d
  | fis,8
    #(set-octavation 0)
    a'32 cis fis a <fis fis,>4 fis,,8 cis''32 fis a cis <a a,>4
  | <cis,, cis,>8 gis''32 eis b gis <gis cis,>4
    <d d,>8 d''32 a fis d <fis fis,>4
  | <b,, b,>8 b'32 d fis b <fis fis,>8 fis32 b d fis <d d,>8
    \clef treble
    d32 fis b d <b b,>4
  \clef bass
  | << { s4 <cis, eis,> s <e b gis eis> } \\
       { s4 <gis,, gis,>4 s <cis cis,> } >>
  | <d' a fis d>4( <cis a e cis>) r2
  | r2 r4 <fis, d b a>4
  | <b gis eis b>4 
    << { <a fis cis>~ <a fis cis> } \\ { d, dis } >>
    <gis eis b>
  | <fis c a fis>2 <fis dis b fis>
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Section 7: accompaniment to the climax build %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

hideTuplets = {
  \override TupletBracket #'bracket-visibility = ##f
  \override TupletBracket #'number-visibility = ##f
}

showTuplets = {
  \override TupletBracket #'bracket-visibility = #'if-no-beam
  \override TupletBracket #'number-visibility = ##t
}

mvmtOnePianoSectSevenRight = \relative a' {
  % 151
  | << { a4. a4 gis8 } \\
       { r8. dis16 fis gis a8 d, cis16 d } >>
  | << { a'4.~ a4 g8 } \\
       { cis,4 b8 <b e> b16 cis dis e } >>
  | << { fis4. fis4 gis8 } \\
       { fis8. dis16 <eis cis> <eis d b>-. 
         b8. dis16 <eis c> <eis cis a> } >>
  | << { fis2. } \\
       { a,8 fis16 a <bis dis> <cis eis> 
         <gis bis fis'>8. <dis' a'>16( <bis eis gis> <b dis fis>) } >>
  | <gis cis eis>8 \times 2/3 { cis16( gis' cis } \times 2/3 { eis, cis' gis') }
    \hideTuplets
    \times 2/3 { bis( dis^\simile gis, dis gis gis, bis, gis' dis) }
  | \times 2/3 { e( b bis' cis e, e' cis' b gis) } 
    \times 2/3 { ais( gis cis, fis cis fis, a fis cis) }
  | \times 2/3 { gis( a e' cis e gis fisis gis cis) }
    \times 2/3 { e( cis gis' a fis cis' e cis gis) }
  | \showTuplets
    \times 4/5 { e32( g b e g }
    \hideTuplets
    \times 2/3 { b16 d, g b, e, g) }
    \times 2/3 { fis( ais, cis e, fis ais cis ais, cis) }
  | r2.
  \showTuplets
}

mvmtOnePianoSectSevenLeft = \relative fis, {
  % 151
  | fis16( cis' fis a cis eis) eis,,( b' eis gis b d)
  | e,,( cis' dis eis fis a) e,,( cis'' e g b a)
  | dis,,( a' b c fis a) d,,( gis b d gis b)
  | cis,,( fis a dis gis a) c,,( fis a c dis gis)
  | cis,4. r4.
  | r2. | r2.
  \hideTuplets
  | r4. \times 2/3 { fis,,16( cis' fis cis fis cis' fis, cis' fis) }
  | <b, b,>4. r4.
  \showTuplets
}

%%%%%%%%%%%%%%%%%%%%%
% Section 8: Climax %
%%%%%%%%%%%%%%%%%%%%%

mvmtOnePianoSectEightRight = \relative b' {
  % 177
  | <b b,>8 <d d,> <fis fis,> <b b,> <ais ais,> <gis gis,> <fis fis,>4
  | <g g,>8 <fis fis,> <e e,> <fis fis,> <d d,> <cis cis,> <fis, fis,>4
  | <g g,>8 <c c,> <e e,> <g g,> <fis fis,> <e e,> <fis fis,>4
  | <g, g,>8 <e e,> <d' d,> <b b,> <g' e b g> r <ais e cis ais> r
  
  | <b fis d b>4 
    \times 2/3 { <gis gis,>16 <g g,> <fis fis,> } <d d,> <cis cis,>
    \times 2/3 { <gis gis,>   <g g,> <fis fis,> } <d d,> <cis cis,>
    b,32( d fis b d fis b d)
  | <b' fis ees c>4
    \times 2/3 { <a a,>16 <gis gis,> <g g,> } <e e,> <dis dis,>
    \times 2/3 { <a a,>16 <gis gis,> <g g,> } <e e,> <dis dis,>
    c,32( e g c e g c e)
  | <b' fis e cis b>4
    \times 2/3 { <ais ais,>16 <a a,> <gis gis,> } <f f,> <e e,>
    \times 2/3 { <ais, ais,>16 <a a,> <gis gis,> } <f f,> <e e,>
    d,32( g b d g b d g)
  | <b g e c b>4 
    \times 2/3 { gis16( g fis } \times 2/3 { eis e dis }
    d32[ cis c b ais a gis g]
    \times 2/3 { fis eis e } \times 2/3 { dis d cis } \times 2/3 { bis b ais } \times 2/3 { a gis g) }

  % 185
  | fis4 ais16 cis fis ais e d' bis gis cis,4
  | \appoggiatura { cis8[ d e] } <fis fis,>8-. <g' e b g>-.
    <ais, ais,>16 <cis cis,> <fis fis,> <ais ais,>
    <e e,> <d' d,> <bis bis,> <gis gis,> <cis, cis,>4
  | \appoggiatura { cis8[ d e] } <fis fis,>-. <g' e cis b g>-.
    <fis, cis ais fis>4
    \appoggiatura { cis8[ d e] } <fis fis,>-. <g' e cis b g>-.
    <fis, cis ais fis>4
  | \appoggiatura { cis8[  d e] } <fis fis,>4
    \appoggiatura { cis'8[ d e] } <fis fis,>4
    \appoggiatura { cis8[  d e] } <fis fis,>4
    \appoggiatura { cis8[  d e] } <fis fis,>4~
  | <fis fis,>2\fermata r2\fermata
}

mvmtOnePianoSectEightLeft = \relative b, {
  % 177
  | b8 d fis b ais gis fis4
  | g8 fis e fis d cis fis,4
  | g8 c e g fis e fis4
  | g,8 e d' b <cis cis,> r <fis fis,> r
  
  | <b fis d b>4 
    \times 2/3 { <gis gis,>16 <g g,> <fis fis,> } <d d,> <cis cis,>
    \times 2/3 { <gis gis,>   <g g,> <fis fis,> } <d d,> <cis cis,>
    b,32( d fis b d fis b d)
  | <b' fis ees c>4
    \times 2/3 { <a a,>16 <gis gis,> <g g,> } <e e,> <dis dis,>
    \times 2/3 { <a a,>16 <gis gis,> <g g,> } <e e,> <dis dis,>
    c,32( e g c e g c e)
  | <b' fis e cis b>4
    \times 2/3 { <ais ais,>16 <a a,> <gis gis,> } <f f,> <e e,>
    \times 2/3 { <ais, ais,>16 <a a,> <gis gis,> } <f f,> <e e,>
    d,32( g b d g b d g)
  | <b g e c b>4 
    \times 2/3 { gis16( g fis } \times 2/3 { eis e dis }
    d32[ cis c b ais a gis g]
    \times 2/3 { fis eis e } \times 2/3 { dis d cis } \times 2/3 { bis b ais } \times 2/3 { a gis g) }
  | <fis' fis,>4 
    <ais ais,>16 <cis cis,> <fis fis,> <ais ais,>
    <e e,> <d' d,> <bis bis,> <gis gis,> 
    <cis, cis,>4
  | \appoggiatura { cis,8[ d e] } <fis fis,>8-. <g' e b g>-.
    <ais, ais,>16 cis fis ais
    <e e,> d' bis gis
    cis,4
  | \appoggiatura { cis8[ d e] } <fis fis,>8-. <g' e cis b g>-.
    <fis, cis ais fis>4
    \appoggiatura { cis8[ d e] } <fis fis,>8-. <g' e cis b g>-.
    <fis, cis ais fis>4
  | \appoggiatura { cis8[  d e] } <fis fis,>4
    \appoggiatura { cis'8[ d e] } <fis fis,>4
    \appoggiatura { cis8[  d e] } <fis fis,>4
    \appoggiatura { cis8[  d e] } <fis fis,>4~
  | <fis fis,>2\fermata r2\fermata
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
  \time 4/4
  R1*4
  \mvmtOnePianoSectFiveRight
  R1*8
  \mvmtOnePianoSectSixRight
  \time 6/8
  R2.*14
  \mvmtOnePianoSectSevenRight
  R2.*17
  \time 4/4
  \mvmtOnePianoSectEightRight
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
  \time 4/4
  R1*4
  \mvmtOnePianoSectFiveLeft
  R1*8
  \mvmtOnePianoSectSixLeft
  \time 6/8
  R2.*14
  \mvmtOnePianoSectSevenLeft
  R2.*17
  \time 4/4
  \mvmtOnePianoSectEightLeft
}
