from JetEngine import *

# Create a new JetCOW opened for read/write
cow = JetCOW("world.cow", False, True)

# New scene, and reference it in the world header
scene = cow.newScene()
cow.getHeader().setAttrInt("MainScene", scene.id)
print "New scene"

# Add a light
light = cow.newDict("DynamicPointLight")
light.setAttrFloat("tx", -100)
light.setAttrFloat("ty", 100)
light.setAttrFloat("tz", 100)
light.setAttrFloat("Intensity", 0.8)
scene.attachChild(light)
light.unref()
print "Added light"

# Add skybox to the scene
skybox = cow.newDict("Skybox")
scene.attachChild(skybox)
skybox.unref()
print "Added skybox"

# Green superllipsoid
surf = cow.newDict("Surface")
gen = cow.newDict("SuperellipsoidGenerator")
gen.setAttrFloat("Radius",50)
gen.setAttrFloat("xe",0.4)
gen.setAttrFloat("ye",0.4)
gen.setAttrFloat("ze",0.4)
cmod = cow.newDict("ColorModulator")
cmod.setAttrFloat("Red", 0.6)
cmod.setAttrFloat("Green", 1.0)
cmod.setAttrFloat("Blue", 0.6)
cmod.setAttrInt("Parent", gen.id)
surf.setAttrInt("Generator", cmod.id)
surf.setAttrFloat("tz",-400)
scene.attachChild(surf)
gen.unref()
cmod.unref()
surf.unref()
print "Added green superellipsoid"

# Yellow superllipsoid
surf = cow.newDict("Surface")
gen = cow.newDict("SuperellipsoidGenerator")
gen.setAttrFloat("Radius",50)
gen.setAttrFloat("xe",1.0)
gen.setAttrFloat("ye",0.2)
gen.setAttrFloat("ze",1.0)
cmod = cow.newDict("ColorModulator")
cmod.setAttrFloat("Red", 1.0)
cmod.setAttrFloat("Green", 1.0)
cmod.setAttrFloat("Blue", 0.0)
cmod.setAttrInt("Parent", gen.id)
surf.setAttrInt("Generator", cmod.id)
surf.setAttrFloat("tx",-150)
surf.setAttrFloat("tz",-400)
scene.attachChild(surf)
gen.unref()
cmod.unref()
surf.unref()
print "Added yellow superellipsoid"

# Red superllipsoid
surf = cow.newDict("Surface")
gen = cow.newDict("SuperellipsoidGenerator")
gen.setAttrFloat("Radius",50)
gen.setAttrFloat("xe",3.0)
gen.setAttrFloat("ye",3.0)
gen.setAttrFloat("ze",3.0)
cmod = cow.newDict("ColorModulator")
cmod.setAttrFloat("Red", 1.0)
cmod.setAttrFloat("Green", 0.2)
cmod.setAttrFloat("Blue", 0.2)
cmod.setAttrInt("Parent", gen.id)
surf.setAttrInt("Generator", cmod.id)
surf.setAttrFloat("tx",150)
surf.setAttrFloat("tz",-400)
scene.attachChild(surf)
gen.unref()
cmod.unref()
surf.unref()
print "Added red superellipsoid"

# Unreference scene, flush JetCOW to disk
scene.unref()
cow.flush()

