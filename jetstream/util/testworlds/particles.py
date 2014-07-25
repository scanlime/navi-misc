from JetEngine import *

# Create a new JetCOW opened for read/write
cow = JetCOW("world.cow", False, True)

# New scene, and reference it in the world header
scene = cow.newScene()
cow.getHeader().setAttrInt("MainScene", scene.id)

# Add skybox to the scene
skybox = cow.newDict("Skybox")
scene.attachChild(skybox)
skybox.unref()

# Add particlesystem to the scene
psys = cow.newDict("ParticleSystem")
psys.setAttrInt("Size", 300)
scene.attachChild(psys)
psys.unref()

# Unreference scene, flush JetCOW to disk
scene.unref()
cow.flush()

