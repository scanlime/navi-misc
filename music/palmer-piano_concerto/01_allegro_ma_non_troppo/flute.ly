\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneFlute = \relative c'' {
  \time 4/4
  \key b \minor

  | R1*30^\allegroMaNonTroppo
  
  % 31
  | << { r8 b'-.\mp r fis-. r d-. r ais-. } \\
       { r8 d-. r b-. r fis-. r cis-. } >>
  | r8 fis r16 e b' e \times 2/3 { b' a b } a e bis e g c
  | \times 2/3 { fis e fis } e a, fis r8. r2
  | R1*11
  
  % 45
  | gis,2\mp a4( cis)
  | fis8 r r4 r2
  | R1*7
  
  % 54
  | << { gis8\mf } \\ { cis,8 } >>
    r8 r4 r4 cis16 d dis e
  | eis8 r r4 r2
  | r2 r8 fis16\f fis fis4~
  | fis8 fis16 fis fis8 e dis cis r16 gis cis gis'
  | b1
  | b1
  | R1*9
  % 69
  \time 6/8
  | R2.*11^\andante
  
  % 80
  | b4.\mp( g
  | e) r
  | des'\mf( aes
  | ees) r
  | R2.*4

  % 88
  | r4. gis~\mf
  \time 4/4
  | << { gis1^\tempoI } { s4\> s4 s4 s4\! } >>
  | R1*11

  % 101
  | c,8\f( ees g c) b( a) bes( aes)
  | a( c, ees g) fis( ees d f)
  | ees4 r4 r2
  | R1*2

  % 106
  | des16\mp( f) f f e( c') c c ces,( ees) ees ees d( bes') bes bes
  | ges bes ees, ges f8-. r e16 gis cis, e fis8-. r
  | R1*29
  % 137
  | \time 6/8
  | R2.*8^\andante

  % 145
  | cis4.\mp cis4 dis8
  | cis4.~ cis4 b8
  | ais2.
  | R2.*4

  % 152
  | r4. r16\mf 
        << { fis'16\< fis\!\> fis-. fis-. fis-.\! } \\
           { a,16 a a-. a-. a-. } >>
  | << { fis'16-. } \\ { b,16-. } >> r16 r4 r4.
  | r8 r16\mf\< a dis eis fis-.\! r16 r4
  | r16 cis\mf\>( gis') gis-. gis-. gis-. gis-.\! gis-. r4
  | r8 r16 fis-.\mf fis-. fis-. fis4.:16-.
  | R2.*3

  % 160
  | b,4\mp \trill ais32( b dis gis ais gis dis b cis[ dis fis b dis fis]) r16
  | R2.*8

  % 168
  | r4. r16 fis,\f fis-. g-. g-. g-.
  | r4. r16 cis\< d-. cis-. cis-. cis-.\!
  | b8 r g fis4 e8
  | d4 b'8 ais4 cis8
  | b4 r8 \repeat "unfold" 3 { fis32 g fis e }
  | \repeat "unfold" 6 { fis32 g fis e }
  | \repeat "unfold" 3 { d32 e d cis } \repeat "unfold" 3 { b32 cis b ais }
  | \repeat "unfold" 3 { b32 cis b ais } \repeat "unfold" 3 { ais32 b ais gis }
  
  % 177
  \time 4/4
  | b8\f^\moderatoMaestoso r r4 r2
  | r1
  | << { g'8 r c r ais r cis r } \\
       { c,8 r e r fis r fis r } >>
  | << { b r g r g r fis r } \\
       { e r b r b r ais r } >>
  | R1*8
  | R1*25^\moderato

  % 214
  | gis'1\mf
  | e4 dis~ dis e8 fis32( gis ais b
  | cis2) cis4 b(
  | d2) r
  | R1*2
  | R1*8^\tempoI
  | \bar "|."
}
