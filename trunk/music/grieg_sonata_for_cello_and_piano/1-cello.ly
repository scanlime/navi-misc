allegroAgitatoCello = \relative c {
	\time 2/2
	\clef bass

	<<
		\tag #'part <<
			\set fontSize = #-2
			\times 2/3 {
				\override TupletBracket #'bracket-visibility = ##f
				\override TupletBracket #'number-visibility = ##f
				\stemUp
				e8_"Piano" [<a c> e]    <a c> [e <a c>]    e8 [<a c> e]    <a c> [e <a c>]
			}
		\\
			r1
		>>
		\tag #'score r1
	>>

	e2 ( f2
	e2 ~ e8) a8 (b c)
	e,4 e4 (f4) f4 (
	e2 ~ e8) c'8 (d e)
	e,2 (f2)
	e2 (b'2)
	e,2 (e'2) ~
	e8 f8 \(e d \acciaccatura {c16[ d]} c8 b c d\)
	e,2 (e2)
	e2 ~ e8 e (f g
	e4 e) e (e)
	e2 ~ e8 g8 (a bes)
	a2 (bes2)
	a2 (d2)
	a2 (f'2) ~
	f1 ~
	f1 ~
	f1 ~
	f2 r2
	r1
	r8 c8  b a gis (a) g fis
	r8 c'8 b a gis (a) g fis
	r8 c'8 b a gis (a) g f
	e (f) e d c (d) c b

	% A
	a2 d,2
	a'2 r2
	r4 a d, d
	a' a r2
	r4 e' d d,
	r e' b' b,
	r e e' e,,
	r1
	a2 e2
	a2 r2
	r4 a e e
	a a r2
	r4 d d, r
	r f' f, r
	r e' e, e
	a'2 ~ \( a8 gis\) a b
	c2 ~ \( c8 b\) c d
	r4 e e e

	\clef treble

	% B
	a2 a2
	a2 ~ \(a8 a8\) b c
	a4 ~ a4 a a
	a1
	\clef bass
	a,2 a2
	a2 ~ \(a8 a8\) b c
	a4 ~ a4 a a
	a1
	a,1 ~
	a4 a (b c)
	a1 ~
	a4 a (b c)
	a1
	a1
	a1 ~
	a4 r4 r2

	% C
}
