\include "01_allegro_ma_non_troppo/text.ly"

mvmtOneCello = \relative c, {
	\time 4/4
	\clef bass
	\key b \minor

	R1*6
	f4 \f r r r
	f4 r f r
	g4 r r r
	fis-. g-. gis-. ais-.
	R1*4
	b16 (d cis d e d cis d b d ees d cis d) e (d)
}
