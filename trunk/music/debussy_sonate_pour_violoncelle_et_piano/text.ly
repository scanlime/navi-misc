prologueTempo = \markup {
  \large\bold "Lent ("
  48 à 54
  \large\bold "="
  \note #"4" #1
  \large\bold ")"
  Sostenuto e molto resoluto
}

raisePrologueTempo = \markup {
  \raise #5 \prologueTempo
}

cedez = \markup {
  \large\bold "Cédez"
}

raiseCedez = \markup {
  \large\bold \raise #2 "Cédez"
}

pocoAnimando = \markup {
  \large\bold "Poco animando"
}

auMouvt = \markup {
  \large\bold "au Mouv"
  \raise #0.5 t
}

serenadeTempo = \markup {
  \large\bold "Modérément animé ("
  72
  \large\bold "="
  \note #"4" #1
  \large \bold ")"
}
