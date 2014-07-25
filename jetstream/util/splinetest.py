from JetEngine import *
cow = JetCOW("world2.cow", False, True)
x = cow.newSpline()
x.set1(0,0,-50)
x.set1(1,0,-25)
x.set1(1,1,50)
x.set1(2,2,-25)
x.set1(3,0,25)
engine.world.scene.attachChild(x)
