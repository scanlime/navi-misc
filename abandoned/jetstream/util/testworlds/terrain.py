from JetEngine import *

# Create a new JetCOW opened for read/write
cow = JetCOW("world.cow", False, True)

# New scene, and reference it in the world header
scene = cow.newScene()
cow.getHeader().setAttrInt("MainScene", scene.id)
print "New scene"

# Add a light at the origin
light = cow.newDict("DynamicPointLight")
light.setAttrFloat("Intensity", 1)
scene.attachChild(light)
light.unref()
print "Added light"

# Add skybox to the scene
skybox = cow.newDict("Skybox")
scene.attachChild(skybox)
skybox.unref()
print "Added skybox"

distance = -200
height = -30
size = 50

# Now a quad with some landscape
#gen = cow.newDict("QuadGenerator")
#gen.setAttrFloat("ax", -size)
#gen.setAttrFloat("az", size)
#gen.setAttrFloat("bx", -size)
#gen.setAttrFloat("bz", -size)
#gen.setAttrFloat("cx", size)
#gen.setAttrFloat("cz", -size)
#gen.setAttrFloat("dx", size)
#gen.setAttrFloat("dz", size)

# Sphere
gen = cow.newDict("SphereGenerator")
gen.setAttrFloat("Radius", size)

disp = cow.newDict("PerlinDisplacement")
disp.setAttrInt("Parent",gen.id)
disp.setAttrFloat("Amplitude", 1)
disp.setAttrFloat("Fundamental", 0.08)
disp.setAttrInt("Octaves", 3)
disp.setAttrFloat("Persistence", 0.8)

surf = cow.newSceneNode("Surface")
surf.setAttrFloat("ty", height)
surf.setAttrFloat("tz", distance)
surf.setAttrInt("Generator", disp.id)
scene.attachChild(surf)
surf.unref()
disp.unref()
gen.unref()
print "Added surface"

# Unreference scene, flush JetCOW to disk
scene.unref()
cow.flush()

