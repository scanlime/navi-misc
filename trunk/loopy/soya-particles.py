#
# A spiffy particle system overlay using Soya 3D
#

import os, math, random, sys
import loopy, soya
import soya.pudding as pudding

datadir = os.path.join(os.path.dirname(sys.argv[0]), "data")

class BlueSmokeEmitter(soya.Smoke):
    def __init__(self,parent):
        soya.Particles.__init__(self,parent,nb_max_particles=150)
        self.t = 0

        spark_material = soya.Material()
        spark_material.additive_blending = 1
        spark_material.texture = soya.open_image(os.path.join(datadir, "blue_spark.png"))
        self.material = spark_material

        self.set_colors((1.0, 1.0, 1.0, 1.0),
                        (0.0, 0.0, 0.0, 0.0))
        self.set_sizes((0.3, 0.3),
                       (0.0, 0.0))
        self.auto_generate_particle = 1

    def generate(self, index):
        sx = (random.random() - 0.5) * 0.05
        sy = random.random() * 0.1
        sz = (random.random() - 0.5) * 0.05
        l = 2.0 * random.random()
        self.set_particle(index,
                          l,           # life
                          sx, sy, sz,  # speed
                          0.,0.,0.)    # acceleration

    def advance_time(self, proportion):
        soya.Smoke.advance_time(self, proportion)
        self.t += 0.4 * proportion
        self.set_xyz(math.sin(self.t) * 1.0,
                     math.cos(self.t) * 1.0 - 1.0, 0)

class Overlay(loopy.Overlay):
    def createPuddingWidgets(self):
        root = pudding.core.RootWidget()

        title = pudding.control.SimpleLabel(root)
        title.font = soya.Font(os.path.join(datadir, "geodesic.ttf"), 100, 100)
        title.color = (1.0, 1.0, 1.0, 0.5)
        title.label = "wasabi"
        title.update()
        title.left = (self.resolution[0] - title.width) / 2
        title.top = self.resolution[1] / 4 - title.height / 2

        return root

    def createScene(self):
        self.scene = soya.World()

        particles = BlueSmokeEmitter(self.scene)

        light = soya.Light(self.scene)
        light.set_xyz(0.5, 0.0, 2.0)

        self.camera = soya.Camera(self.scene)
        self.camera.z = 10.0

    def resized(self):
        soya.set_video(self.resolution[0], self.resolution[1], False, True)
        self.camera.resize(0, 0, self.resolution[0], self.resolution[1])
        self.puddingRoot = self.createPuddingWidgets()

    def setup(self):
        soya.init("Loopy Demo", self.resolution[0], self.resolution[1])
        pudding.init()
        self.puddingRoot = self.createPuddingWidgets()
        self.createScene()
        self.idler = soya.Idler(self.scene)

    def render(self):
        self.idler.begin_round()
        self.idler.advance_time(1.0)
        self.idler.end_round()
        self.camera.render()
        self.puddingRoot.render()

if __name__ == "__main__":
    loopy.overlays.append(Overlay())
