\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneCello = \relative c, {
	\time 4/4
	\clef bass
	\key b \minor

	R1*6
	f4 \f r r r
	f4 r f r
	g4 r r r

	% 10
	fis-. g-. gis-. ais-.
	R1*4

	% 15
	b16 (d cis d e d cis d b d ees d cis d) e (d)
	f16 (d cis d b d) fis (cis) g' (dis cisis dis) gis (e d e)
	g16 (d cis d b d cis d) fis (d cis d) f (cis b cis)
	gis16 (cis b cis) gis' (cis, d cis) b' (d, e d) b' (e, f e)
	ais16 fis g fis e g fis e d b cis b ais cis b d

	% 20
	cis16 bis d c ees d e d f e fis eis g fis aes e
	g16 fis a eis gis fis a e g fis ais e g fis a e
	gis (fis ais eis gis fis ais eis gis fis ais eis gis fis ais eis)
	b8-. r8 r4 r2

	R1*18
	r4 f (g a
	d c b e)
	a,2 e

	% 45
	eis fis
	d8-. e'-. e16 d e r r2
	R1*3

	% 50
	r2 r4 g16 c, g8
	\repeat "tremolo" 6 { a16 e16 }  ais16 e16 ais16 e16
	\repeat "tremolo" 6 { a16 e16 }  b'16 e,16 b'16 e,16
	a8 [c r a] r gis [r c]
	c8 r8 r4 r2

	% 55
	r8 cis' b a gis fis e d
	cis d16 d d8 fis r e [r dis]
	r cis fis (e dis cis) r r
	r2 e
	dis1

	% 60
	r1
	e2 (d
	b cis4 e
	gis2.) fis4 (
	dis2 eis)

	% 65
	r4 fis, (a cis
	d2) r2
	r4 f, (a d
	e2) r2 \fermata

	\time 6/8
	R1*6/8*10

	<< {
		s4. bes (
		cis d)
		s a (
		c des)
	} \\ {
		r4. bes (
		a f)
		r a (
		aes fis)
	} >>
	r4. r8 dis'16 e dis8-.->
	gis,8-> r4 r4.

	% 85
	r4. r8 cis16 d cis8-.->
	b8-> r4 r8 a'16 b a8-.->
	d,8-> r4 r8 c16 b a8-.->
	gis8 r4 cis16-> cis cis-. cis-. cis-. cis-.
	\time 4/4
	\times 2/3 {<gis cis>8-> <gis cis> <gis cis>-.} \times 2/3 {<gis cis>-. <gis cis>-. <gis cis>-.} \times 2/3 {<g c>8-> <g c> <g c>-.} \times 2/3 {<g c>-. <g c>-. <g c>-.}

	% 90
	<g c>4\pp ais8->\mf r8 r4 g8-> r8
	f8-> r8 r4 r2
	R1*7
	g8 (b d g) fis (e) f (eis)

	% 100
	e (dis) e (cis) d (cis) d (b)
	c16\mf ees d ees f ees d ees c ees fes ees d ees f ees
	c\ff (ees d ees a, ees' d ees) d (a bes ees d c b a)
	aes8 r8 r4 r4 r16 fis'\mf r8
	g16 r r8 fis,4 (f e

	% 105
	f)
	<< {
		d' (ees c
		des)
	} \\ {
		g, (bes a
		bes)
	} >> r4 r2
	r1
	fis1\p \trill
	fis1 \trill

	R1*19
	r8 fis' r gis r fis r a

	% 130
	r8 aes r f r e r g
	r8 ees r d r fis r bis,
	r1
	r2 r4
	<< {
		a4 (
		b a)
	} \\ {
		a4\mf (
		d, \> dis) \!
	} >>
	r2
	R1*2
	\time 6/8
	R1*6/8*4
	gis4.\mf gis4 (fis8
	b4.) a4.
	\appoggiatura gis8 gis'4. fis4 eis8
	e8. dis8. ~ dis8. bis8.
	cis4. e
	gis d
	cis r
	R1*6/8*5
	r4. r8.\mp cis16 \< cis-. cis-. \!
	<fis,-. cis'>16 r16 r4 r4.
	cis'16-.->\mf cis16-. cis16-. cis16-. cis16-. cis16-. \repeat "tremolo" 6 bis16^\simile
	\repeat "tremolo" 6 b16 \repeat "tremolo" 6 ais
	\repeat "tremolo" 6 a16 \repeat "tremolo" 6 gis
	\repeat "tremolo" 6 g16 \repeat "tremolo" 6 fis
}
