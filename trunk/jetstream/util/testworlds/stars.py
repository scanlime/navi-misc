from JetEngine import *
import random

worldRadius = 5000
starCubeRadius = 800
numStars = 2000
starRadius = 3

# Create a new JetCOW opened for read/write
cow = JetCOW("world.cow", False, True)

# New scene, and reference it in the world header
scene = cow.newScene()
cow.getHeader().setAttrInt("MainScene", scene.id)
print "New scene"

# Add a light at the origin
light = cow.newDict("DynamicPointLight")
light.setAttrFloat("Intensity", 1.2)
scene.attachChild(light)
light.unref()
print "Added light"

# Add an inverted sphere enclosing the world
sphereGen = cow.newDict("SphereGenerator")
sphereGen.setAttrFloat("Radius",worldRadius)
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
print "Added skysphere"

sphereGen = cow.newDict("SphereGenerator")
sphereGen.setAttrFloat("Radius",starRadius)

# Make some color modifiers to tint the "stars" slightly red or blue
colorMods = []
for i in range(20):
    m = cow.newDict("ColorModulator")
    c = random.uniform(-0.2, 0.2)
    m.setAttrFloat("Red",  1 + c)
    m.setAttrFloat("Blue", 1 - c)
    m.setAttrInt("Parent", sphereGen.id)
    colorMods.append(m)

# A ton of spheres at random places, as a stress test
#
# NOTE: Don't do real starfields/forests/swarms like this!
#       Use metasurfaces (which haven't been written yet...)
#
for n in range(numStars):
    surf = cow.newSceneNode("Surface")
    surf.setAttrFloat("tx",random.uniform(-starCubeRadius,starCubeRadius))
    surf.setAttrFloat("ty",random.uniform(-starCubeRadius,starCubeRadius))
    surf.setAttrFloat("tz",random.uniform(-starCubeRadius,starCubeRadius))
    surf.setAttrInt("Generator", random.choice(colorMods).id)
    scene.attachChild(surf)
    surf.unref()
sphereGen.unref()
print "Added %d stars" % numStars

# Unreference scene, flush JetCOW to disk
for m in colorMods:
    m.unref()
scene.unref()
cow.flush()

