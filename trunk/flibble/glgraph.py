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
        for axis in (0,1):
            self.position[axis] += edgeStrength / max(self.position[axis], 1)
            self.position[axis] -= edgeStrength / max(graph.viewport.size[axis] - self.position[axis], 1)

        # Random wandering
        self.position += (random.normalvariate(0, graph.temperature),
                          random.normalvariate(0, graph.temperature))

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
        self.temperature = 30

    def addNode(self, *args, **kwargs):
        node = Node(*args, **kwargs)
        self.nodes.append(node)
        return node

    def findNode(self, key):
        """Find a node, given a node instance, text, or index"""
        if type(key) is int:
            return self.nodes[key]
        if type(key) is str:
            for node in self.nodes:
                if node.text == key:
                    return node
            return self.addNode(key)
        return key

    def connectNodes(self, node1, node2, *args, **kwargs):
        edge = Edge(self.findNode(node1),
                    self.findNode(node2),
                    *args, **kwargs)
        self.edges.append(edge)

    def render(self):
        # For a simulated simulated-annealing effect, gradually
        # decrease the 'temperature', the amount of random node movement.
        self.temperature *= 0.98

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


class GraphController:
    def __init__(self, viewport, graph):
        self.viewport = viewport
        self.graph = graph
        viewport.onMouseMotion.observe(self.handleMotion)
        viewport.onDrawFrame.observe(self.animate)
        viewport.onMouseButtonDown.observe(self.handlePress)
        viewport.onMouseButtonUp.observe(self.handleRelease)
        self.selectedNode = None
        self.dragActive = False
        self.mousePosition = None

    def handlePress(self, event):
        if event.button == 1:
            self.dragActive = True
            self.mousePosition = event.pos

    def handleRelease(self, event):
        if event.button == 1:
            self.dragActive = False

    def handleMotion(self, event, minDistance=70):
        self.mousePosition = event.pos

        # Find the closest node if a drag isn't active yet
        if not self.dragActive:
            closestNode = None
            closestDist = minDistance ** 2
            for node in self.graph.nodes:
                v = node.position - event.pos
                d2 = Numeric.dot(v,v)
                if d2 < closestDist:
                    closestDist = d2
                    closestNode = node
            self.selectNode(closestNode)

    def animate(self):
        if self.dragActive and self.selectedNode:
            # Attract the selected node to the mouse
            self.selectedNode.position[:] = self.mousePosition

    def selectNode(self, node):
        if self.selectedNode:
            self.selectedNode.circleColor = self.selectedNode.__class__.circleColor
        self.selectedNode = node
        if node:
            node.circleColor = (0.5, 1, 0.5)


def main():
    # Basic PyBZEngine setup
    loop = Event.EventLoop()
    viewport = Viewport.OpenGLViewport(loop)
    view = ThreeDRender.View(viewport)

    # Add a background image
    viewport.mode = Viewport.GL.UnclearedMode()
    background = HUD.Background(viewport, Texture.load("bg.jpeg"))

    graph = Graph(viewport)
    gc = GraphController(viewport, graph)

    graph.connectNodes("Fruit", "Vegetable")
    graph.connectNodes("Vegetable", "Anthill")
    graph.connectNodes("Anthill", "Green")
    graph.connectNodes("Anthill", "Water")
    graph.connectNodes("Water", "Eggs")
    graph.connectNodes("Water", "Moose")
    graph.connectNodes("Moose", "Vegetable")

    graph.connectNodes("Banana", "Celery")
    graph.connectNodes("Shovel", "Optical")

    loop.run()

if __name__ == '__main__':
    main()

### The End ###
