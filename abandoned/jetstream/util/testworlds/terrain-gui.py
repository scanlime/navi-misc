#
# Utility applet for experimenting with noise parameters in realtime
# This must be run using "./jetstream -e util/testworlds/terrain-gui.py"
# It will present a GUI for interactively manipulating the noise parameters,
# then a world.cow with that terrain will be saved.
#

import engine, input, PicoGUI
from PicoGUI import XWTParser
from debug import *

import sys
sys.path.append('util/testworlds')
import solarsystem


class TerrainControl(DebugPanel):
    def __init__(self):
        DebugPanel.__init__(self, "Terrain Control")
        cow = engine.world.cow
        scene = engine.world.scene
        app = engine.globals.app

        # Add some scenery from solarsystem.py
        solarsystem.addOriginLight(cow,scene)
        solarsystem.addWorldSphere(cow,scene)
        #solarsystem.addPlanets(cow,scene,300)

        # Sphere
        gen = cow.newDict("SphereGenerator")
        gen.setAttrFloat("Radius", 50)

        # Quad
        #gen = cow.newDict("QuadGenerator")
        #r = 50
        #h = -20
        #gen.setAttrFloat("ax", -r)
        #gen.setAttrFloat("ay",  h)
        #gen.setAttrFloat("az", -r)
        #gen.setAttrFloat("bx",  r)
        #gen.setAttrFloat("by",  h)
        #gen.setAttrFloat("bz", -r)
        #gen.setAttrFloat("cx",  r)
        #gen.setAttrFloat("cy",  h)
        #gen.setAttrFloat("cz",  r)
        #gen.setAttrFloat("dx", -r)
        #gen.setAttrFloat("dy",  h)
        #gen.setAttrFloat("dz",  r)

        # Perlin noise displacement across the surface
        disp = cow.newDict("PerlinDisplacement")
        disp.setAttrInt("Parent",gen.id)

        # Default noise parameters
        disp.setAttrFloat("Amplitude", 1)
        disp.setAttrFloat("Fundamental", 0.08)
        disp.setAttrInt("Octaves", 1)
        disp.setAttrFloat("Persistence", 0.8)

        # Color modulator
        cmod = cow.newDict("ColorModulator")
        cmod.setAttrInt("Parent", disp.id)

        # Surface using the sphere and noise
        surf = cow.newSceneNode("Surface")
        surf.setAttrFloat("tz", -200)
        surf.setAttrInt("Generator", cmod.id)

        # Cleanup
        scene.attachChild(surf)
        cmod.unref()
        surf.unref()
        disp.unref()
        gen.unref()

        # Adjusters for the noise parameters
        self.adjusters = [
            JetCOWFloatAdjuster(self, disp, "Amplitude", 50),
            JetCOWFloatAdjuster(self, disp, "Fundamental", 0.1),
            JetCOWIntAdjuster(self, disp, "Octaves", 20),
            JetCOWFloatAdjuster(self, disp, "Persistence", 1.5),
            JetCOWFloatAdjuster(self, gen, "Radius", 100),
            JetCOWIntAdjuster(self, disp, "Seed", 0x7FFF),
            JetCOWFloatAdjuster(self, cmod, "Red", 2),
            JetCOWFloatAdjuster(self, cmod, "Green", 2),
            JetCOWFloatAdjuster(self, cmod, "Blue", 2),
            ]

    def destroy(self):
        for i in self.adjusters:
            i.destroy()
        DebugPanel.destroy(self)


# Run the EngineGlobals, World, and Input modules with our extra terrain control
engine.globals = engine.EngineGlobals()
engine.world = engine.World()
engine.input = input.Input()
engine.world.create()
TerrainControl()
engine.globals.run()
engine.world.scene.unref()
engine.world.cow.flush()
engine.globals.shutdown()

### The End ###
