#
# Simple force-directed graph layout engine, written in Pyrex
#

cdef extern from "stdlib.h":
    ctypedef int size_t
    void* malloc(size_t size)
    void free(void* ptr)
    long int random()

cdef struct ArrayVertex:
    float x
    float y

cdef struct ArrayEdge:
    int a
    int b


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
