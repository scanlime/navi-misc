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

	%45
	eis fis
	d8-. e' e16 d e r r2

}
