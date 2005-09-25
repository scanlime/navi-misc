import os, math, random
import loopy, soya
import soya.pudding as pudding

class BlueSmokeEmitter(soya.Smoke):
    def __init__(self,parent):
        soya.Particles.__init__(self,parent,nb_max_particles=150)
        self.t = 0

        spark_material = soya.Material()
        spark_material.additive_blending = 1
        spark_material.texture = soya.open_image("blue_spark.png")
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
        r = 1.0

        self.set_xyz(math.sin(self.t)*r,
                     math.cos(self.t)*r - r,
                     0)

class SoyaOverlay(loopy.Overlay):
    def setup():
        soya.init()
        print "argv: %r" % (sys.argv,)
        soya.path.append(os.path.join(os.path.dirname(sys.argv[0]), "data"))

        self.scene = soya.World()
        self.scene.atmosphere = soya.NoBackgroundAtmosphere()

        #pudding.init()
        #w = pudding.core.RootWidget()

        #l = pudding.control.SimpleLabel(w)
        #l.font = soya.Font("geodesic.ttf", 100, 100)
        #l.label = "wasabi"
        #l.update()
        #l.left = (w.width - l.width) / 2
        #l.top = w.height / 4 - l.height / 2
        #print l.top, l.left

        self.particles = BlueSmokeEmitter(self.scene)

        self.light = soya.Light(self.scene)
        self.light.set_xyz(0.5, 0.0, 2.0)

        self.camera = soya.Camera(self.scene)
        self.camera.z = 10.0

        self.idler = soya.Idler(scene)

    def render():
        self.idler.begin_round()
        self.idler.advance_time(1.0)
        self.idler.end_round()
        self.camera.render()

loopy.overlays.append(SoyaOverlay())
