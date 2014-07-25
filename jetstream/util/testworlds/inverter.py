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
light.setAttrFloat("tz", -100)
light.setAttrFloat("Intensity", 1.2)
scene.attachChild(light)
light.unref()
print "Added light"

# Add skybox to the scene
skybox = cow.newDict("Skybox")
scene.attachChild(skybox)
skybox.unref()
print "Added skybox"

# Add a sphere with a SurfaceInverter
sphereGen = cow.newDict("SphereGenerator")
sphereGen.setAttrFloat("Radius",50)
inv = cow.newDict("SurfaceInverter")
inv.setAttrInt("Parent", sphereGen.id)
surf = cow.newSceneNode("Surface")
surf.setAttrFloat("tz", -200)
surf.setAttrInt("Generator", inv.id)
scene.attachChild(surf)
surf.unref()
sphereGen.unref()
inv.unref()
print "Added inverted sphere"

# Unreference scene, flush JetCOW to disk
scene.unref()
cow.flush()

