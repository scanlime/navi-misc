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
}
