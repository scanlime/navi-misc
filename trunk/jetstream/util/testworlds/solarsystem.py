# You'll have to excuse the crudity of this model,
# I didn't have time to build it to scale or to paint it.

from JetEngine import *
import random


# Add a light at the origin
def addOriginLight(cow, scene):
    light = cow.newDict("DynamicPointLight")
    light.setAttrFloat("Intensity", 1)
    scene.attachChild(light)
    light.unref()
    print "Added light"

# Add an inverted sphere enclosing the world
def addWorldSphere(cow, scene, worldRadius=5000):
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

def addPlanets(cow, scene, numPlanets=1000, planetRadius=5, planetCubeRadius=1200):
    sphereGen = cow.newDict("SphereGenerator")
    sphereGen.setAttrFloat("Radius",planetRadius)

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
    for n in range(numPlanets):
        # Generate terrain for each planet, with a different seed
        disp = cow.newDict("PerlinDisplacement")
        disp.setAttrInt("Parent", random.choice(colorMods).id)
        disp.setAttrInt("Seed", int(random.random()*(2**31)))
        disp.setAttrFloat("Amplitude", 0.1)
        disp.setAttrFloat("Fundamental", 0.8)
        disp.setAttrInt("Octaves", 3)
        disp.setAttrFloat("Persistence", 0.8)

        surf = cow.newSceneNode("Surface")
        surf.setAttrFloat("tx",random.uniform(-planetCubeRadius,planetCubeRadius))
        surf.setAttrFloat("ty",random.uniform(-planetCubeRadius,planetCubeRadius))
        surf.setAttrFloat("tz",random.uniform(-planetCubeRadius,planetCubeRadius))
        surf.setAttrInt("Generator", disp.id)
        scene.attachChild(surf)

        disp.unref()
        surf.unref()
    sphereGen.unref()
    for m in colorMods:
        m.unref()
    print "Added %d planets" % numPlanets


if __name__ == '__main__':
    # Create a new JetCOW opened for read/write
    cow = JetCOW("world.cow", False, True)

    # New scene, and reference it in the world header
    scene = cow.newScene()
    cow.getHeader().setAttrInt("MainScene", scene.id)
    print "New scene"

    addOriginLight(cow,scene)
    addWorldSphere(cow,scene)
    addPlanets(cow,scene)

    # Unreference scene, flush JetCOW to disk
    scene.unref()
    cow.flush()

