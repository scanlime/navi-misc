from soya import opengl as gl
import soya

a = 0

def _frame():
    global a

    gl.glDisable(gl.GL_TEXTURE_2D)
    gl.glEnable(gl.GL_BLEND)

    gl.glPushMatrix()
    gl.glLoadIdentity()

    gl.glRotatef(a, 1.0, 0.2, 0.0)
    a += 2

    gl.glColor4f(0.2, 0.2, 0.2, 1.0)
    gl.glBegin(gl.GL_QUADS)
    s = 0.25
    gl.glVertex3f(-s,-s,0)
    gl.glVertex3f(s,-s,0)
    gl.glVertex3f(s,s,0)
    gl.glVertex3f(-s,s,0)
    gl.glEnd()

    gl.glPopMatrix()
    gl.glDisable(gl.GL_BLEND)
    gl.glEnable(gl.GL_TEXTURE_2D)

def frame():
    try:
        _frame()
    except:
        import traceback
        traceback.print_exc()

