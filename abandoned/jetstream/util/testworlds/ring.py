from JetEngine import *
import math

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

# Add skybox to the scene
skybox = cow.newDict("Skybox")
scene.attachChild(skybox)
skybox.unref()
print "Added skybox"

# Make a bunch of spheres forming a ring 600 units wide about the origin in the xz plane.
n = 0
t = 0
sphereGen = cow.newDict("SphereGenerator")
sphereGen.setAttrFloat("Radius",50)
while t < math.pi*2:
    surf = cow.newSceneNode("Surface")
    surf.setAttrFloat("tx",600*math.cos(t))
    surf.setAttrFloat("tz",600*math.sin(t))
    surf.setAttrInt("Generator", sphereGen.id)
    scene.attachChild(surf)
    surf.unref()
    n += 1
    t += 0.3
sphereGen.unref()
print "Added %d spheres in a ring" % n

# Unreference scene, flush JetCOW to disk
scene.unref()
cow.flush()

