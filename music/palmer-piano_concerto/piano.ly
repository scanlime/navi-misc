\include "01_allegro_ma_non_troppo/piano.ly"

% from lilypond-documentation
% pasted here because I can't figure out how to factor it out

\version "2.3.22"

\book {
	\header {
		composer = "LUKE PALMER"
		title = "CONCERTO"
	}
    \score {
     \context PianoStaff <<
       \context Staff=pianoRight \mvmtOnePianoRight
       \context Dynamics=pianoDynamics \mvmtOnePianoDynamics
       \context Staff=pianoLeft \mvmtOnePianoLeft
       \context Dynamics=pedal \mvmtOnePianoPedal
     >>
     \layout {
       \context {
         \type "Engraver_group_engraver"
         \name Dynamics
         \alias Voice % So that \cresc works, for example.
         \consists "Output_property_engraver"

         minimumVerticalExtent = #'(-1 . 1)
         pedalSustainStrings = #'("Ped." "*Ped." "*")
         pedalUnaCordaStrings = #'("una corda" "" "tre corde")

         \consists "Piano_pedal_engraver"
         \consists "Script_engraver"
         \consists "Dynamic_engraver"
         \consists "Text_engraver"

         \override TextScript #'font-size = #2
         \override TextScript #'font-shape = #'italic
         \override DynamicText #'extra-offset = #'(0 . 2.5)
         \override Hairpin #'extra-offset = #'(0 . 2.5)

         \consists "Skip_event_swallow_translator"

         \consists "Axis_group_engraver"
       }
       \context {
         \PianoStaff
         \accepts Dynamics
         \override VerticalAlignment #'forced-distance = #7
       }
     }
     \midi {
       \context {
         \type "Performer_group_performer"
         \name Dynamics
         \consists "Piano_pedal_performer"
         \consists "Span_dynamic_performer"
         \consists "Dynamic_performer"
       }
       \context {
         \PianoStaff
         \accepts Dynamics
       }
     }
   }
}
