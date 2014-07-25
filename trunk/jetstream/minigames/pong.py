#
# Pong minigame using the jetstream engine
#

import engine, input, PicoGUI
from PicoGUI import XWTParser
from debug import *

engine.globals = engine.EngineGlobals()
engine.world = engine.World()
engine.input = input.Input()
engine.world.create("pong.cow")
cow = engine.world.cow
scene = engine.world.scene

# Add a light
light = cow.newDict("DynamicPointLight")
light.setAttrFloat("Intensity", 1.2)
light.setAttrFloat("tx", -20)
light.setAttrFloat("tz", 300)
scene.attachChild(light)
light.unref()

# Add a skysphere
sphereGen = cow.newDict("SphereGenerator")
sphereGen.setAttrFloat("Radius",500.0)
inv = cow.newDict("SurfaceInverter")
inv.setAttrInt("Parent", sphereGen.id)
cmod = cow.newDict("ColorModulator");
cmod.setAttrFloat("Red", 0.0)
cmod.setAttrFloat("Green", 0.0)
cmod.setAttrFloat("Blue", 0.0)
cmod.setAttrInt("Parent", inv.id)
surf = cow.newSceneNode("Surface")
surf.setAttrInt("Generator", cmod.id)
scene.attachChild(surf)
cmod.unref()
inv.unref()
surf.unref()
sphereGen.unref()

dotGenSphere = cow.newDict("SphereGenerator")
dotGenSphere.setAttrFloat("Radius",1)
dotGen = cow.newDict("ColorModulator")
dotGen.setAttrFloat("Red", 1.0)
dotGen.setAttrFloat("Green", 1.0)
dotGen.setAttrFloat("Blue", 0.7)
dotGen.setAttrInt("Parent", dotGenSphere.id)
dotGenSphere.unref()

s = cow.newSceneNode("Surface")
s.setAttrFloat("tx", 0)
s.setAttrFloat("ty", 0)
s.setAttrFloat("tz", 0)
s.setAttrInt("Generator", dotGen.id)
scene.attachChild(s)
s.unref()

dotGen.unref()

engine.globals.run()
engine.world.scene.unref()
engine.world.cow.flush()
engine.globals.shutdown()

### The End ###
