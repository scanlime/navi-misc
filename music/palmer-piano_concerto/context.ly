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
    \type "Engraver_group_engraver"
    \name Tempo
    \alias Voice % So that \cresc works, for example.
    \consists "Output_property_engraver"

    minimumVerticalExtent = #'(0 . 0)

    \consists "Text_engraver"
    \override TextScript #'font-size = #2
    \override TextScript #'extra-offset = #'(0 . 2)

    \consists "Skip_event_swallow_translator"
    \consists "Axis_group_engraver"
  }
  
  \context {
    \PianoStaff
    \accepts Dynamics
    \accepts Tempo
    \override VerticalAlignment #'forced-distance = #7
  }

  \context {
    \Staff
    \accepts Tempo
  }

  \context {
    \Score
    \accepts Tempo
  }
}
