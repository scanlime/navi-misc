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
}
