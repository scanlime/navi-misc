#!/usr/bin/env python

import sys, os, os.path, soya
from random import random
import soya.pudding as pudding

soya.init("Particle-fu", 1280, 720)
soya.path.append(os.path.dirname(sys.argv[0]))

scene = soya.World()

pudding.init()
w = pudding.core.RootWidget()
soya.set_root_widget(w)

l = pudding.control.SimpleLabel(w)
l.font = soya.Font("geodesic.ttf", 100, 100)
l.label = "wasabi"
l.update()
l.left = (w.width - l.width) / 2
l.top = w.height / 4 - l.height / 2

spark_material = soya.Material()
spark_material.additive_blending = 1
spark_material.texture = soya.open_image("blue_spark.png")

class MyParticleSystem(soya.Smoke):
    def __init__(self,parent):
        soya.Particles.__init__(self,parent,nb_max_particles=450)

        self.material = spark_material

        self.set_colors((1.0, 1.0, 1.0, 1.0),
                        (0.0, 0.0, 0.0, 0.0))
        self.set_sizes((0.5, 0.5),
                       (0.0, 0.0))
        self.auto_generate_particle = 1

    def generate(self, index):
        sx = (random() - 0.5) * 0.1
        sy = (random() - 0.5) * 0.1
        sz = (random() - 0.5) * 0.1
        l = 30.0 * random()
        self.set_particle(index,
                          l,           # life
                          sx, sy, sz,  # speed
                          0.,0.,0.)    # acceleration

particles=MyParticleSystem(scene)

light = soya.Light(scene)
light.set_xyz(0.5, 0.0, 2.0)

camera = soya.Camera(scene)
camera.z = 10.0
w.add_child(camera)

idler = soya.Idler(scene)
idler.min_frame_duration = 1.0 / 120
idler.idle()


