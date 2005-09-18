#!/usr/bin/env python

import sys, os, os.path, soya, math
from random import random
import soya.pudding as pudding

soya.init("Particle-fu", 1280, 720)
soya.path.append(os.path.dirname(sys.argv[0]))

scene = soya.World()

pudding.init()
w = pudding.core.RootWidget()
soya.set_root_widget(w)

l = pudding.control.SimpleLabel(w)
l.color = (1.0, 1.0, 1.0, 0.2)
fontSize = w.height // 8
l.font = soya.Font("geodesic.ttf", fontSize, fontSize)
l.label = "wasabi"
l.update()
l.left = (w.width - l.width) / 2
l.top = w.height / 3 - l.height / 2

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

particles = BlueSmokeEmitter(scene)

light = soya.Light(scene)
light.set_xyz(0.5, 0.0, 2.0)

camera = soya.Camera(scene)
camera.z = 10.0
w.add_child(camera)

idler = soya.Idler(scene)
idler.min_frame_duration = 1.0 / 120
idler.idle()


