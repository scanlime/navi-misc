\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneBass = \relative c {
	\time 4/4
	\clef bass
	\key b \minor

	R1*6^\allegroMaNonTroppo
	b4\f r r r
	b r cis r
	c r r2
	b4 b b fis'-.

	% 10
	R1*4
	b8\mf r r4 b8 r r4

	% 15
	r4 b8 r r4 b8 r
	r4 b8 r r4 cis8 r
	gis8 r r4 gis'8 r r4
	fis8 r r4
	b,8 r r4

	% 20
	fis1 ~
	fis1 ~
	\< <fis cis'> \!
	<b, fis'>8 r r4 r2
	R1*11

	% 35
	r4 r r a'^\pizz
	d, r a r
	ais r b a
	gis gis' g r
	fis r f e

	% 40
	a, r a' g
	fis r b, d
	R1*4
	r8 <e, e'>8\f^\arco r4 r2
	R1*3

	% 50
	<a a'>8 [<d d'> r <cis cis'>] r <c c'> r4
	R1*2
	r8 cis\f r a r gis r cis
	c8 r r4 r2

	% 55
	r8 cis'\p \< b a gis fis e d \!
	cis8\f r r fis r e r dis
	r cis fis (e dis cis) r4
	r2 e
	dis1

	% 60
	R1*2
	d2\mp (cis
	dis e)
	fis (gis)

	% 65
	r4\mf \< fis, (a cis \!
	d2) r2
	r4\mf \< f, (a d \!
	e2) r2\fermata

	\time 6/8
	R1*6/8*14
	r4. r8 dis16 e dis8->
	gis,8-> r4 r4.

	% 85
	r4. r8 cis16 d cis8->
	b8-> r4 r8 a'16 b a8->
	d,8 r4 r8 c16 b a8->
	\setTextDecresc
	gis8-> r4 cis16-> \> cis \! cis-. cis-. cis-. cis-.
	\time 4/4
	\once \override TextScript #'extra-offset = #'(0 . 1.0)
	\times 4/6 {cis8->^\tempoI cis8 cis-. cis-. cis-. cis-.}
	\times 4/6 {d8-> d8 d-. d-. d-. d-.}

	% 90
	c4\pp <aes aes'>->\mf^\pizz r <a a'>4->
	<fis fis'>4-> r r2
	R1
	b4\f r r2
	b4 r fis r

	% 95
	e r r2
	e4 r b' fis
	R1*5
	c'16\ff (ees d ees a, ees' d ees) d (a bes ees d c bes a)
	aes8 r8 r4 r2
	R1

	% 105
	r4^\oneBass b\mp ees a,
	f'4 r r2
	R1
	fis,1\p \trill
	\> fis1 \trill \!

	% 110
	R1*10 | R1*9
	<< {
		r8 fis'^\divisi r gis r fis r a
		r aes r f r e r g
		r ees r d r fis r bis,
	} \\ {
		r8 fis\ff r gis r fis r a
		r aes r f r e r g
		r ees r d r fis r bis,
	} >>
	R1*8

	% 140
	b4\mf^\pizz r a r
	gis r r2
	gis r gis r
}
