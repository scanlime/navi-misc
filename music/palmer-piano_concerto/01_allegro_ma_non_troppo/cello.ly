\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneCello = \relative c, {
	\time 4/4
	\clef bass
	\key b \minor

	R1*6
	fis4 \f r r r
	fis4 r fis r
	g4 r r r

	% 10
	fis-. g-. gis-. ais-.
	R1*4

	% 15
	b16\mp (d cis d e d cis d b d ees d cis d) e (d)
	f16 (d cis d b d) fis (cis) g' (dis cisis dis) gis (e dis e)
	g16 (d cis d b d cis d) fis (d cis d) f (cis b cis)
	gis16 (cis b cis) gis' (cis, d cis) b' (d, e d) b' (e, f e)
	ais16 fis g fis e g fis e d b cis b ais cis b d

	% 20
	cis16 bis d cis ees d e d f e fis eis g fis aes eis
	g16 fis a eis gis fis a eis gis fis ais eis gis fis ais eis
	gis \< (fis ais eis gis fis ais eis gis fis ais eis gis fis ais \! eis)
	b8-.\f^\solo r8 r4 r2

	R1*18
	r4 fis^\oneCello\mf (g a
	d \< c b e) \!
	a,2 e

	% 45
	eis fis
	\once \override TextScript #'extra-offset = #'(0 . 0.5)
	d8-.^\tutti e'-. e16 d e r r2
	R1*3

	% 50
	r2 r4 g16\mf c, g8
	\repeat "tremolo" 6 { a16 e16 }  ais16 e16 ais16 e16
	\repeat "tremolo" 6 { a16 e16 }  b'16 e,16 b'16 e,16
	a8 cis\f [r a] r gis [r cis]
	c8 r8 r4 r2

	% 55
	r8 cis'\p \< b a gis fis e d \!
	cis\f d16 d d8 fis r e [r dis]
	r cis fis (e dis cis) r r
	r2 e
	dis1

	% 60
	R1^\menoMosso
	e2\mp (d
	b cis4 e
	gis2.) fis4 (
	dis2 eis)

	% 65
	r4 fis,\mf \< (a cis \!
	d2) r2
	r4 f,\mf \< (a d \!
	e2^\solo) r2 \fermata

	\time 6/8
	R1*6/8
	R1*6/8*9

	<< {
		s4.^\divisi bes (
		cis d)
		s a (
		c des)
	} \\ {
		r4.\p \< bes \! (
		a f)
		r\p \< a \! (
		aes fes)
	} >>
	r4. r8 dis'16\f e dis8-.->
	gis,8-> r4 r4.

	% 85
	r4. r8 cis16 d cis8-.->
	b8-> r4 r8 a'16 b a8-.->
	d,8-> r4 r8 c16 b a8-.->
	\setTextDecresc
	gis8 r4 cis16^\accent \> cis \! cis-. cis-. cis-. cis-.
	\setHairpinDecresc
	\time 4/4
	\once \override TextScript #'extra-offset = #'(0 . 1.0)
	\times 4/6 {<gis cis>8-> [<gis cis> <gis cis>-. <gis cis>-. <gis cis>-. <gis cis>-.]}
        \times 4/6 {<g c>8-> [<g c> <g c>-. <g c>-. <g c>-. <g c>-.]}

	% 90
	<g c>4\pp aes8->\mf r8 r4 g8-> r8
	fis8-> r8 r4 r2
	R1*7
	\once \override DynamicText #'extra-offset = #'(0 . -0.5)
	g8\f (b d g) fis (e) f (ees)

	% 100
	e (dis) e (cis) d (cis) d (b)
	c16\mf^\tutti ees d ees f ees d ees c ees fes ees d ees f ees
	c\ff (ees d ees a, ees' d ees) d (a bes ees d c bes a)
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
	R1
	fis1\p \trill
	\> fis1 \! \trill

	R1*10 | R1*9
	r8 fis'\ff r gis r fis r a

	% 130
	r8 aes r f r e r g
	r8 ees r d r fis r bis,
	R1
	r2 r4
	<< {
		a4 (
		b a)
	} \\ {
		\once \override DynamicText #'extra-offset = #'(-1.5 . -1.5)
		a4\mf (
		\once \override Hairpin #'extra-offset = #'(0 . -1.0)
		d, \> dis) \!
	} >>
	r2
	R1*2
	\mark \markup {
		\fontsize #-3 {
		        \note #"2" #1
		        \large \bold "="
	        	\note #"4." #1
		}
	}
	\time 6/8
	\once \override TextScript #'extra-offset = #'(-2.0 . 0.5)
	R1*6/8*4
	gis4.\mf^\oneCello gis4 (fis8
	b4.) a4.
	\appoggiatura gis8 gis'4. fis4 eis8
	e8. dis8. ~ dis8. bis8.
	cis4. e
	gis d
	cis r
	R1*6/8*5
	r4.^\tutti r8.\mp cis16 \< cis-. cis-. \!
	<fis,-. cis'>16 r16 r4 r4.
	cis'16-.->\mf cis-. cis-. cis-. cis-. cis-. \repeat "tremolo" 6 bis16^\simile
	\repeat "tremolo" 6 b16 \repeat "tremolo" 6 ais
	\repeat "tremolo" 6 a16 \repeat "tremolo" 6 gis
	\repeat "tremolo" 6 g16 \< \repeat "tremolo" 6 fis \!
	b16\mf \tenuto b\tenuto b-. b-. b-. b-. \repeat "tremolo" 6 fisis16^\simile
	\repeat "tremolo" 6 gis16 \repeat "tremolo" 6 fis16
	\repeat "tremolo" 6 e16
	<< {
		\repeat "tremolo" 6 b'16^\divisi
		\repeat "tremolo" 6 g16 fis fis fis fis e e
		\repeat "tremolo" 6 d'16 \repeat "tremolo" 6 d16
		\repeat "tremolo" 6 e16 \repeat "tremolo" 6 e16
		\repeat "tremolo" 6 c16 \repeat "tremolo" 6 b16
		\repeat "tremolo" 6 b16 \repeat "tremolo" 6 cis16
	} \\ {
		\repeat "tremolo" 6 dis,16
		\repeat "tremolo" 6 cis cis cis cis cis cis cis
		\repeat "tremolo" 6 fis16 \repeat "tremolo" 6 g16
		\repeat "tremolo" 6 g16 \repeat "tremolo" 6 fis16
		\repeat "tremolo" 6 fis16 \repeat "tremolo" 6 e16
		\repeat "tremolo" 6 fis16 \repeat "tremolo" 6 fis16
	} >>
	r4. r16\mf <fis b>16 \< <fis b> <fis cis'> <fis cis'> <fis cis'> \!
	R1*6/8
	r4. r16\f <fis b>16 \< <fis b> <fis cis'> <fis cis'> <fis cis'> \!
	r4. r16\f <fis b>16 \< <fis b> <fis cis'> <fis cis'> <fis cis'> \!
	<fis b>8 fis32 (g fis e \repeat "unfold" 3 {fis32 g fis e} fis32 g fis e)
	\repeat "percent" 4 {fis32 (g fis e \repeat "unfold" 4 {fis32 g fis e} fis32 g fis e) }
	\once \override Hairpin #'extra-offset = #'(0 . -1.5)
	fis32 \< (g fis e fis32 g fis e fis32 g fis e) fis8 fis fis \!

	\time 4/4
	% 177
	<< {
		b8^\divisi r d r e r fis r
		g r gis r gis r ais r
		g r g r fis r fis r
		b, r b r cis r fis, r
	} \\ {
		b8\ff r fis r ais r ais r
		b r b r bis r cis r
		e r c r d r cis r
		e, r e r cis' r fis, r
	} >>
	R1*15
	dis2\p^\oneCello cis
	cis'4 (ais) dis,2
	gis \< ais \!
	<< {
		ais'1^\twoCellos
		fis2 eis
		fis4 cis dis2
		cis1
	} \\ {
		dis2. \> cisis4 \!
		cis4 b ~ b cis
		fis,2 dis
		\> <cis gis'>1 \!
	} >>
	R1*14
	b'4\p^\tuttiPizz ais a gis
	\repeat "unfold" 2 {b4 ais a gis}
	g fis f e
	e r r2
	R1*6
	\bar "|."
}
