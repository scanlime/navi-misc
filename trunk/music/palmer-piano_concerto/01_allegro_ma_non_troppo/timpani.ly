\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneTimpani = \relative c {
	\time 4/4
	\clef bass
	\key b \minor

	R1*2^\allegroMaNonTroppo
	b4\f r4 r2
	b4 r fis r

	% 5
	e r r2
	e4 r b' fis
	b\f r r2
	b4 r fis r
	e r r2

	% 10
	b'4 b fis fis
	r2 fis4 r
	r2 fis4 r
	r2 fis4 r
	fis r fis r

	% 15
	b r b r
	r b r b
	r b r b
	e, r e r
	fis r b r

	% 20
	R1*26

	% 46
	r8 e,8\ff ~ e4 r2
	R1*7

	% 54
	cis4\f r r2
	cis4 r gis'16\mp \< gis gis gis gis gis gis gis \!
	cis,4.\f^\menoMosso r8 r2
	R1*2
	r8 cis'16 cis cis8 cis,16 cis cis8 r8 r cis16 cis

	% 60
	cis4 r r2
	R1*8

	\time 6/8
	R1*6/8*20^\andante
	\time 4/4
	R1^\tempoI

	% 90
	r4 aes'\mf r g
	fis r r2
	R1
	b4\f r r2
	b4 r fis r

	% 95
	e r r2
	e4 r b' fis8 \times 2/3 {fis16 fis fis}
	b4\f fis r fis
	e r r2
	R1*3
	g8 g g g16 g g8 g16 g g8 g
	R1*26
	fis4->\f fis fis fis

	% 130
	\repeat "percent" 3 {fis4 fis fis fis}
	cis r cis r
	R1*3

	\time 6/8
	R1*6/8*21^\andante
	r4. \times 2/3 {fis16\mp \< fis fis fis fis fis fis fis fis\!}
	b8\ff r4 r4.
	R1*6/8*7
	\repeat "percent" 3 { b8.\ff b16 b b fis4.}

	% 170
	b8. b16 b b fis8. fis16 fis fis
	b8 fis'4 ~ fis4.
	\repeat "percent" 4 {fis2.}
	fis4. \< fis,8 fis fis \!
	\time 4/4
	b4\ff^\moderatoMaestoso fis fis fis
	e e e fis
	e e fis fis

	% 180
	e e e fis
	R1*41
	fis4\f r fis r
	b r r2
	R1*4
	\bar "|."
}
