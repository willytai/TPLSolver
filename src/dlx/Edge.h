#ifndef EDGE_H
#define EDGE_H

#include "Vertex.h"

struct Edge
{
    Edge(Vertex* vv1, Vertex* vv2) : isConflictEdge(false) { v1 = vv1; v2 = vv2; }
    Vertex*     v1;
    Vertex*     v2;
    bool        isConflictEdge;

    // overload cout
    friend ostream& operator << (ostream& os, const Edge& e) {
        os << '(' << *(e.v1) << ", " << *(e.v2) << ')';
        return os;
    }
};

#endif /* EDGE_H */
