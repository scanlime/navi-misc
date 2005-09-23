import sys, os, os.path, soya, math
from random import random
import soya.pudding as pudding
from soya import opengl as gl

class BlueSmokeEmitter(soya.Smoke):
    spark_material = soya.Material()
    spark_material.additive_blending = 1
    spark_material.texture = soya.open_image("blue_spark.png")
    t1 = 0.0
    t2 = 0.0

    def __init__(self,parent):
        soya.Particles.__init__(self,parent,nb_max_particles=150)

        self.material = self.spark_material

        self.set_colors((1.0, 1.0, 1.0, 1.0),
                        (0.0, 0.0, 0.0, 0.0))
        self.set_sizes((0.3, 0.3),
                       (0.0, 0.0))
        self.auto_generate_particle = 1

    def generate(self, index):
        sx = (random() - 0.5) * 0.05
        sy = random() * 0.1
        sz = (random() - 0.5) * 0.05
        l = 2.0 * random()
        self.set_particle(index,
                          l,           # life
                          sx, sy, sz,  # speed
                          0.,0.,0.)    # acceleration

    def advance_time(self, proportion):
        soya.Smoke.advance_time(self, proportion)

        self.t1 += 0.4 * proportion
        r = 1.0

        self.set_xyz(math.sin(self.t1)*r,
                     math.cos(self.t1)*r - r,
                     0)

initted = False
idler = None
camera = None

def init():
    global initted
    global idler
    global camera
    soya.path.append(os.getcwd())

    scene = soya.World()
    scene.atmosphere = soya.NoBackgroundAtmosphere()

    pudding.init()
    w = pudding.core.RootWidget()

    l = pudding.control.SimpleLabel(w)
    l.font = soya.Font("geodesic.ttf", 100, 100)
    l.label = "wasabi"
    l.update()
    l.left = (w.width - l.width) / 2
    l.top = w.height / 4 - l.height / 2
    print l.top, l.left

    particles = BlueSmokeEmitter(scene)

    light = soya.Light(scene)
    light.set_xyz(0.5, 0.0, 2.0)

    camera = soya.Camera(scene)
    camera.z = 10.0
    w.add_child(camera)

    idler = soya.Idler(scene)
    initted = True

def frame():
    if not initted:
        return
    idler.begin_round()
    idler.advance_time(1.0)
    idler.end_round()

    # Clean out the OpenGL state, disable everything the app enabled
    for capability, enabled in gl_enabled.iteritems():
        if enabled:
            gl.glDisable(capability)

    camera.render()

    # Put the GL state back the way we found it
    for capability, enabled in gl_enabled.iteritems():
        if enabled:
            gl.glEnable(capability)
        else:
            gl.glDisable(capability)

def viewport(x, y, width, height):
    initted or soya.init(create_surface=0)

    if soya.get_screen_width() == width and soya.get_screen_height() == height:
        return
    try:
        soya.set_video(width, height, 0, 0)
    except RuntimeError:
        pass
    assert soya.get_screen_width() == width
    assert soya.get_screen_height() == height

    initted or init()
