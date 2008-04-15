CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  prox : "OpticalProximity"
  vp : "ViewPortConduit_20"

PUB main | c

  prox.start(4,5)
  vp.general(string("mode=dso"))
  vp.share(prox.getOutputAddress, prox.getOutputAddress)
  vp.view(string("prox"))
  vp.start

  dira[9]~~

  c := cnt
  repeat
    outa[9]~
    waitcnt(c += clkfreq/10)

    outa[9]~~
    waitcnt(c += clkfreq/20)