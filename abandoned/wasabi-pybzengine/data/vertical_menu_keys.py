#
# Configuration file for key bindings used by ArcMenu
#

bindings.add(KeyPress, pygame.K_UP).observe(command.spinUp)
bindings.add(KeyPress, pygame.K_DOWN).observe(command.spinDown)

bindings.add(MousePress, 4).observe(command.spinUp)
bindings.add(MousePress, 5).observe(command.spinDown)

bindings.add(MousePress, 1).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_SPACE).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_RETURN).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_RIGHT).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_LEFT).observe(command.cancel)
bindings.add(KeyPress, pygame.K_ESCAPE).observe(command.cancel)

from Wasabi import IR
bindings.add(IR.ButtonPress, 'up').observe(command.spinUp)
bindings.add(IR.ButtonPress, 'down').observe(command.spinDown)
bindings.add(IR.ButtonPress, 'enter').observe(command.selectCurrent)
bindings.add(IR.ButtonPress, 'play').observe(command.selectCurrent)
bindings.add(IR.ButtonPress, 'right').observe(command.selectCurrent)
bindings.add(IR.ButtonPress, 'left').observe(command.cancel)
bindings.add(IR.ButtonPress, 'x').observe(command.cancel)
