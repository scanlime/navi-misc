#
# Configuration file for key bindings used by RingMenu
#

bindings.add(KeyPress, pygame.K_LEFT).observe(command.spinLeft)
bindings.add(KeyPress, pygame.K_RIGHT).observe(command.spinRight)

bindings.add(MousePress, 4).observe(command.spinLeft)
bindings.add(MousePress, 5).observe(command.spinRight)

bindings.add(MousePress, 1).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_SPACE).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_RETURN).observe(command.selectCurrent)
bindings.add(KeyPress, pygame.K_UP).observe(command.selectCurrent)

#try:
#    from Wasabi import IR
#    bindings.add(IR.ButtonPress, 'left').observe(command.spinLeft)
#    bindings.add(IR.ButtonPress, 'right').observe(command.spinRight)
#except:
#    pass
from Wasabi import IR
bindings.add(IR.ButtonPress, 'left').observe(command.spinLeft)
bindings.add(IR.ButtonPress, 'right').observe(command.spinRight)
