#!/usr/bin/env python
from BZEngine.UI import Texture, GLText, GLOrtho, Viewport, ThreeDRender, HUD
from BZEngine import Event
from OpenGL import GL
import Numeric, math, random


class Node:
    textHeight = 20
    textColor = (0,0,0)
    shadowOpacity = 0.3
    shadowOffset = (2,2)
    circleColor = (1, 1, 0.8)
    circleMargin = 20
    fontName = None
    position = None
    opacity = 0

    def __init__(self, text):
        self.text = text
        self.circle = None

    def animate(self, graph):
        # If we're new, start in the middle
        if self.position is None:
            self.position = Numeric.array(graph.viewport.size, Numeric.Float)/2

        # Slowly fade in
        if self.opacity < 1:
            self.opacity += 0.01

        # Stay away from other nodes
        for other in graph.nodes:
            if other.position:
                repulsionForce(self, other, 80)

        # Stay away from the viewport edges
        edgeStrength = 200
        self.position[0] += abs(edgeStrength / self.position[0])
        self.position[1] += abs(edgeStrength / self.position[1])
        self.position[0] -= abs(edgeStrength / (graph.viewport.size[0] - self.position[0]))
        self.position[1] -= abs(edgeStrength / (graph.viewport.size[1] - self.position[1]))

        # Random wandering
        self.position += (random.normalvariate(0, 0.2),
                          random.normalvariate(0, 0.2))

    def render(self):
        # Lazy texture loading
        if not self.circle:
            self.circle = Texture.load("circle.png")

        GLOrtho.setup()
        GLOrtho.push()
        GLOrtho.translate(*self.position)

        # Draw an ellipse around our text
        textWidth, textHeight = GLText.size(self.text, self.textHeight, self.fontName)
        GLOrtho.push()
        GLOrtho.translate(-textWidth/2 - self.circleMargin,
                          -textHeight/2 - self.circleMargin)
        GLOrtho.setTexture(self.circle)
        GLOrtho.setColor(*self.circleColor + (self.opacity,))
        GLOrtho.filledRect((textWidth + 2*self.circleMargin,
                            textHeight + 2*self.circleMargin))
        GLOrtho.pop()

        # Draw the text shadow
        GLOrtho.setColor(*self.textColor + (self.shadowOpacity * self.opacity,))
        GLOrtho.push()
        GLOrtho.translate(*self.shadowOffset)
        GLText.draw(self.text, self.textHeight, self.fontName, (0.5, 0.5))
        GLOrtho.pop()

        # Draw the text itself
        GLOrtho.setColor(*self.textColor + (self.opacity,))
        GLText.draw(self.text, self.textHeight, self.fontName, (0.5, 0.5))
        GLOrtho.pop()


def springForce(node1, node2, length, strength):
    try:
        v = node2.position - node1.position
        l = math.sqrt(Numeric.dot(v,v))
        f = v * (length - l) * strength
        node1.position -= f
        node2.position += f
    except OverflowError:
        pass


def repulsionForce(node1, node2, strength):
    try:
        v = node2.position - node1.position
        l2 = Numeric.dot(v,v)
        f = v / (l2 + 1) * strength
        node1.position -= f
        node2.position += f
    except OverflowError:
        pass


class Edge:
    def __init__(self, node1, node2, length=200):
        self.node1 = node1
        self.node2 = node2
        self.length = length

    def animate(self, graph):
        springForce(self.node1, self.node2, self.length, 0.001)

    def render(self):
        opacity = self.node1.opacity * self.node2.opacity
        GLOrtho.setup()
        GLOrtho.setColor(0,0,0,opacity)
        GL.glEnable(GL.GL_LINE_SMOOTH)
        GL.glBegin(GL.GL_LINE_LOOP)
        GL.glVertex2f(*self.node1.position)
        GL.glVertex2f(*self.node2.position)
        GL.glEnd()


class Graph:
    def __init__(self, viewport, nodes=(), edges=()):
        self.nodes = list(nodes)
        self.edges = list(edges)
        self.viewport = viewport
        viewport.onDrawFrame.observe(self.render)

    def addNode(self, *args, **kwargs):
        node = Node(*args, **kwargs)
        self.nodes.append(node)
        return node

    def connectNodes(self, node1, node2, *args, **kwargs):
        if type(node1) is int:
            node1 = self.nodes[node1]
        if type(node2) is int:
            node2 = self.nodes[node2]
        edge = Edge(node1, node2)
        self.edges.append(edge)

    def render(self):
        # Let the edges and nodes animate themselves
        for node in self.nodes:
            node.animate(self)
        for edge in self.edges:
            edge.animate(self)

        # Draw everything
        for edge in self.edges:
            edge.render()
        for node in self.nodes:
            node.render()


def main():
    # Basic PyBZEngine setup
    loop = Event.EventLoop()
    viewport = Viewport.OpenGLViewport(loop)
    view = ThreeDRender.View(viewport)

    # Add a background image
    viewport.mode = Viewport.GL.UnclearedMode()
    background = HUD.Background(viewport, Texture.load("bg.jpeg"))

    graph = Graph(viewport)

    graph.addNode("Fruit")
    graph.addNode("Vegetable")
    graph.addNode("Anthill")
    graph.addNode("Water")
    graph.addNode("Moose")
    graph.connectNodes(0,1)
    graph.connectNodes(1,2)
    graph.connectNodes(2,3)
    graph.connectNodes(3,4)
    graph.connectNodes(4,2)

    graph.connectNodes(graph.addNode("Banana"), graph.addNode("Celery"))
    graph.connectNodes(graph.addNode("Shovel"), graph.addNode("Optical"))


    loop.run()

if __name__ == '__main__':
    main()

### The End ###
