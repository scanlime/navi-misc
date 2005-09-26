#
# This is a Loopy version of Soya's "basic-1" tutorial. See
# the original tutorial for full documentation, this is just
# intended to show the changes necessary to run in Loopy.
#
# See soya-particles for a slightly larger example that uses
# pudding and shows animation.
#
import sys, os, soya, loopy

class Overlay(loopy.Overlay):
    def setup(self):
        soya.init("Loopy Demo", *self.resolution)
        soya.path.append(os.path.join(os.path.dirname(sys.argv[0]), "data"))
        scene = soya.World()

        sword_model = soya.Shape.get("sword")
        sword = soya.Volume(scene, sword_model)
        sword.x = 1.0
        sword.rotate_lateral(90.0)

        light = soya.Light(scene)
        light.set_xyz(0.5, 0.0, 2.0)

        self.camera = soya.Camera(scene)
        self.camera.z = 2.0

    def resized(self):
        self.camera.resize(0, 0, *self.resolution)

    def render(self):
        self.camera.render()

if __name__ == "__main__":
    loopy.overlays.append(Overlay())
