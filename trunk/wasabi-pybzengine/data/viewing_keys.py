#
# Configuration file for key bindings used by ThreeDControl.Viewing
#

bindings.add(KeyPress, 'f').observe(command.toggleFullscreen)
bindings.add(KeyPress, 'c').observe(command.toggleCameraInfo)
bindings.add(KeyPress, 'w').observe(command.toggleWireframe)
bindings.add(KeyPress, 'x').observe(command.toggleXRay)
bindings.add(KeyPress, 'r').observe(command.toggleRecorder)
bindings.add(KeyPress, 'h').observe(command.toggleFrameRate)
bindings.add(KeyPress, 'q').observe(command.quit)
bindings.add(KeyPress, pygame.K_ESCAPE).observe(command.quit)
bindings.add(MouseWheel, 0.1, 'any').observe(command.zoom)
bindings.add(KeyAxis, '=', '-', 0.1).observe(command.zoom)
bindings.add(KeyAxis, '[', ']', 0.1).observe(command.fovZoom)

dragButton = 3
bindings.add(MouseGrab, dragButton)
bindings.add(MouseDrag, dragButton, None, 0.2).observe(command.rotate)
bindings.add(MouseDrag, dragButton, pygame.KMOD_SHIFT).observe(command.pan)
bindings.add(MouseDrag, dragButton, pygame.KMOD_CTRL).observe(command.lift)

from Wasabi import IR
bindings.add(IR.ButtonPress, 'filter').observe(command.toggleXRay)
bindings.add(IR.ButtonPress, 'display').observe(command.toggleFrameRate)
bindings.add(IR.ButtonPress, 'v1').observe(command.toggleCameraInfo)
