#
# Simple force-directed graph layout engine, written in Pyrex
#

cdef extern from "stdlib.h":
    ctypedef int size_t
    void* malloc(size_t size)
    void free(void* ptr)
    long int random()

cdef extern from "math.h":
    float sqrtf(float x)

cdef struct ArrayVertex:
    float x
    float y

cdef struct ArrayEdge:
    int a
    int b


cdef float fabs(float x):
    if x < 0:
        return -x
    return x


cdef float applySpring(ArrayVertex* v1, ArrayVertex* v2, float naturalLength, float stiffness):
    """Apply spring forces between the given vertices, return
       the amount of energy left in the system.
       """
    cdef ArrayVertex ab, unitv, force
    cdef float length, forceMag

    # Subtract v2 from v1, to get a vector pointing from v1 to v2
    ab.x = v1.x - v2.x
    ab.y = v1.y - v2.y

    # Calculate the magnitude of ab, the current length of the spring
    length = sqrtf(ab.x * ab.x + ab.y * ab.y)

    # Calculate the magnitude of the spring force
    forceMag = (length - naturalLength) * stiffness

    # Calculate the actual force using ab's direction and forceMag
    force.x = ab.x / length * forceMag
    force.y = ab.y / length * forceMag

    # Apply that force to the two vertices
    v1.x = v1.x - force.x
    v1.y = v1.y - force.y
    v2.x = v2.x + force.x
    v2.y = v2.y + force.y
    return fabs(forceMag)


cdef float repel(ArrayVertex* v1, ArrayVertex* v2, float strength):
    """Apply a repulsion force between the given vertices, return
       the amount of energy left in the system.
       """
    cdef ArrayVertex ab, unitv, force
    cdef float distance, forceMag, distanceSquared

    # Subtract v2 from v1, to get a vector pointing from v1 to v2
    ab.x = v1.x - v2.x
    ab.y = v1.y - v2.y

    # Calculate the magnitude of ab, the current distance of the spring
    distanceSquared = ab.x * ab.x + ab.y * ab.y
    distance = sqrtf(distanceSquared)

    # Calculate the magnitude of the spring force (inverse squared law)
    forceMag = -strength / distanceSquared

    # Calculate the actual force using ab's direction and forceMag
    force.x = ab.x / distance * forceMag
    force.y = ab.y / distance * forceMag

    # Apply that force to the two vertices
    v1.x = v1.x - force.x
    v1.y = v1.y - force.y
    v2.x = v2.x + force.x
    v2.y = v2.y + force.y
    return fabs(forceMag)


cdef class Graph:
    """Represents a graph as an array of ArrayVertex structures holding
       the position of each vertex, and an array of ArrayEdge structures
       holding the indices of each connected vertex pair.

       The graph should be constructed with a sequence of 2-tuples specifying
       vertex indices, and the number of total vertices the graph uses.
       """
    cdef int numVertices
    cdef int numEdges
    cdef ArrayVertex *vertices
    cdef ArrayEdge *edges

    def __new__(self, edges, numVertices):
        cdef int i

        self.numEdges = len(edges)
        self.edges = <ArrayEdge*> malloc(sizeof(ArrayEdge) * self.numEdges)
        if self.edges == NULL:
            raise MemoryError

        for i from 0 <= i < self.numEdges:
            self.edges[i].a = edges[i][0]
            self.edges[i].b = edges[i][1]

        self.numVertices = numVertices
        self.vertices = <ArrayVertex*> malloc(sizeof(ArrayVertex) * self.numVertices)
        if self.vertices == NULL:
            raise MemoryError

    def __dealloc__(self):
        free(self.vertices)
        free(self.edges)

    def __repr__(self):
        return "<Graph with %d vertices and %d edges>" % (self.numVertices, self.numEdges)

    def randomize(self, float xMin, float yMin, float xMax, float yMax):
        """Randomize all vertex positions uniformly within the given rectangle"""
        cdef int i
        cdef float width, height
        width = xMax - xMin
        height = yMax - yMin

        for i from 0 <= i < self.numVertices:
            self.vertices[i].x = random() / 2147483647.0 * width + xMin
            self.vertices[i].y = random() / 2147483647.0 * height + yMin

    def getVectors(self):
        """Get a list of vertex position vectors, as 2-tuples"""
        l = []
        for i from 0 <= i < self.numVertices:
            l.append((self.vertices[i].x, self.vertices[i].y))
        return l

    def iteration(self):
        """Run one iteration of the spring system simulation,
           returning the amount of energy left in the system.
           """
        cdef ArrayEdge *edge
        cdef float energy
        cdef int i, j

        energy = 0

        # Apply spring forces between the vertices of each edge
        for i from 0 <= i < self.numEdges:
            edge = &self.edges[i]
            energy = energy + applySpring(&self.vertices[edge.a], &self.vertices[edge.b],
                                          50, 0.1)

        # Apply much longer springs to all other pairs of vertices
        for i from 0 <= i < self.numVertices:
            for j from i < j < self.numVertices:
                energy = energy + repel(&self.vertices[i], &self.vertices[j], 200)

        return energy

