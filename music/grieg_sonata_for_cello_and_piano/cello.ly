\include "1-cello.ly"

\book {
	\header {
		composer = "EDVARD GRIEG, Op. 36"
		title = "SONATA"
	}
	\score {
		\header {
			piece = "I"
		}
		\context Staff = allegroAgitatoCello <<
			\set Staff.midiInstrument = "cello"
			\keepWithTag #'part \allegroAgitatoCello
		>>
		\midi {
			\tempo 2=100
		}
		\layout {}
	}
}
