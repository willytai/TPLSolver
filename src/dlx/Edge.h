#ifndef EDGE_H
#define EDGE_H

#include "Vertex.h"

struct Edge
{
    Edge(int vv1, int vv2) { v1_id = vv1; v2_id = vv2; if (v1_id > v2_id) ::swap(v1_id, v2_id); }

    int         v1_id;
    int         v2_id;

    // overload cout
    friend ostream& operator << (ostream& os, const Edge& e) {
        os << '(' << e.v1_id << ", " << e.v2_id << ')';
        // os << '(' << *(e.v1) << ", " << *(e.v2) << ')';
        return os;
    }
};

#endif /* EDGE_H */
